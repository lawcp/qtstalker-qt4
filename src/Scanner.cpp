/*
 *  Qtstalker - Commodity/Stock Charting and Technical Analysis Platform
 *
 *  Copyright (C) 2001-2009 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include <qlayout.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <q3hgroupbox.h>
#include <q3progressdialog.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <Q3TextStream>
#include <stdlib.h>
#include <qlabel.h>
#include "Scanner.h"
#include "BarData.h"
#include "SymbolDialog.h"
#include "DbPlugin.h"
#include "IndicatorPlugin.h"
#include "HelpWindow.h"
#include "Traverse.h"


Scanner::Scanner (QString n, DBIndex *i) : Q3TabDialog (0, 0, FALSE)
{
  resize(640, 540);

  chartIndex = i;
  scannerName = n;

  QString s = "Qtstalker Scanner";
  s.append(": ");
  s.append(scannerName);
  setCaption (s);

  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  Q3HGroupBox *gbox = new Q3HGroupBox(tr("Symbol Selection"), w);
  vbox->addWidget(gbox);

  allSymbols = new QCheckBox(tr("All symbols"), gbox);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));

  fileButton = new QPushButton(tr("0 Symbols"), gbox);
  connect(fileButton, SIGNAL(clicked()), this, SLOT(getSymbols()));

  basePath = new QComboBox(gbox);
  basePath->insertItem(tr("Chart"), -1);
  basePath->insertItem(tr("Group"), -1);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 2);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("Bar Length"), w);
  grid->addWidget(label, 0, 0);

  BarData bd(scannerName);
  period = new QComboBox(w);
  bd.getBarLengthList(barLengthList);
  period->insertStringList(barLengthList, -1);
  period->setCurrentText("Daily");
  grid->addWidget(period, 0, 1);

  label = new QLabel(tr("Bars"), w);
  grid->addWidget(label, 1, 0);

  bars = new QSpinBox(1, 99999999, 1, w);
  bars->setValue(100);
  grid->addWidget(bars, 1, 1);

  list = new FormulaEdit(w, FormulaEdit::Logic);
  vbox->addWidget(list);

  setDefaultButton(tr("&Scan"));
  connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(scan()));

  setApplyButton(tr("&Apply"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveRule()));

  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(exitDialog()));

  setOkButton(QString::null);

  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(slotHelp()));

  addTab(w, tr("Parms"));

  loadRule();
}

Scanner::~Scanner ()
{
}

void Scanner::scan ()
{
  if (! fileList.count() && ! allSymbols->isChecked())
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("No symbols selected."));
    return;
  }

  // open the CUS plugin
  QString iplugin("CUS");
  IndicatorPlugin *plug = config.getIndicatorPlugin(iplugin);
  if (! plug)
  {
    config.closePlugin(iplugin);
    return;
  }

  QString s;
  list->getText(s);
  QStringList l = QStringList::split("\n", s, FALSE);
  plug->setCustomFunction(l);

  this->setEnabled(FALSE);

  // clear dir for scan symbols
  QDir dir;
  config.getData(Config::GroupPath, s);
  s.append("/Scanner");
  if (! dir.exists(s))
    dir.mkdir(s, TRUE);
  s.append("/" + scannerName);
  if (! dir.exists(s))
    dir.mkdir(s, TRUE);
  else
  {
    int loop;
    dir.setPath(s);
    for (loop = 2; loop < (int) dir.count(); loop++)
    {
      QString s2 = dir.absPath() + "/" + dir[loop];
      if (! dir.remove(s2))
        qDebug("%s not removed", s2.latin1());
    }
  }

  if (allSymbols->isChecked())
  {
    QString ts;
    if (! basePath->currentText().compare(tr("Chart")))
      config.getData(Config::DataPath, ts);
    else
      config.getData(Config::GroupPath, ts);
    Traverse trav(Traverse::File);
    trav.traverse(ts);
    trav.getList(fileList);
  }

  Q3ProgressDialog prog(tr("Scanning..."),
                       tr("Cancel"),
		       fileList.count(),
		       this,
		       "progress",
		       TRUE);
  prog.resize(640, 84);
  prog.show();

  int minBars = bars->value();

  emit message(QString("Scanning..."));

  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
  {
    prog.setProgress(loop);
    emit message(QString());
    if (prog.wasCancelled())
    {
      emit message(QString("Scan cancelled"));
      break;
    }

    QFileInfo fi(fileList[loop]);
    if (fi.isDir())
      continue;

    DbPlugin db;
    QDir dir;
    if (! dir.exists(fileList[loop]))
      continue;
    db.open(fileList[loop], chartIndex);

    db.setBarRange(minBars);
    db.setBarLength((BarData::BarLength) barLengthList.findIndex(period->currentText()));

    BarData *recordList = new BarData(fileList[loop]);
    QDateTime dt = QDateTime::currentDateTime();
    db.getHistory(recordList, dt);
    db.close();

    // load the CUS plugin and calculate
    plug->setIndicatorInput(recordList);
    Indicator *i = plug->calculate();
    if (! i->getLines())
    {
      delete recordList;
      delete i;
      continue;
    }

    PlotLine *line = i->getLine(0);
    if (line && line->getSize() > 0)
    {
      if ((line->getData(line->getSize() - 1) > 0) && (! fileList[loop].contains( '^', FALSE )))
      {
        QString ts;
        config.getData(Config::GroupPath, ts);
        QString s = "ln -s \"" + fileList[loop] + "\" " + ts + "/Scanner/" + scannerName;
        system(s);
      }
    }

    delete recordList;
    delete i;

    emit message(QString());
  }

  if (! prog.wasCancelled())
    emit message(QString("Scan complete"));

  config.closePlugin(iplugin);

  this->setEnabled(TRUE);

  emit scanComplete();
}

void Scanner::saveRule ()
{
  QString s;
  config.getData(Config::ScannerPath, s);
  s.append("/" + scannerName);

  QFile f(s);
  if (! f.open(QIODevice::WriteOnly))
    return;
  Q3TextStream stream(&f);

  stream << "allSymbols=" << QString::number(allSymbols->isChecked()) << "\n";
  stream << "compression=" << period->currentText() << "\n";
  stream << "bars=" << bars->text() << "\n";
  stream << "basepath=" << basePath->currentText() << "\n";

  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
    stream << "symbol=" << fileList[loop] << "\n";

  list->getText(s);
  QStringList l = QStringList::split("\n", s, FALSE);
  stream << "script=" << l.join("|") << "\n";

  f.close();
}

void Scanner::loadRule ()
{
  QString s;
  config.getData(Config::ScannerPath, s);
  s.append("/" + scannerName);

  QFile f(s);
  if (! f.open(QIODevice::ReadOnly))
    return;
  Q3TextStream stream(&f);

  fileList.clear();

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (! s.length())
      continue;

    QString key;
    QString dat;
    if (s.contains("="))
    {
      int t = s.find("=", 0, TRUE);
      key = s.left(t);
      dat = s.right(s.length() - t - 1);
    }
    else
      continue;

    if (! key.compare("allSymbols"))
    {
      allSymbols->setChecked(dat.toInt());
      continue;
    }

    if (! key.compare("compression"))
    {
      period->setCurrentText(dat);
      continue;
    }

    if (! key.compare("symbol"))
    {
      fileList.append(dat);
      continue;
    }

    if (! key.compare("bars"))
    {
      bars->setValue(dat.toInt());
      continue;
    }

    if (! key.compare("basepath"))
    {
      if (! dat.compare(tr("Chart")))
        basePath->setCurrentItem(0);
      else
        basePath->setCurrentItem(1);
      continue;
    }

    if (! key.compare("script"))
    {
      QStringList l2 = QStringList::split("|", dat, FALSE);
      int loop;
      for (loop = 0; loop < (int) l2.count(); loop++)
        list->setLine(l2[loop]);
    }
  }

  fileButton->setText(QString::number(fileList.count()) + " Symbols");

  f.close();
}

void Scanner::exitDialog ()
{
  reject();
  emit exitScanner();
}

void Scanner::getSymbols ()
{
  QString s;
  if (! basePath->currentText().compare(tr("Chart")))
    config.getData(Config::DataPath, s);
  else
    config.getData(Config::GroupPath, s);
  QString s2("*");
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s,
  					  s,
					  s2,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select symbols to scan"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    fileList = dialog->selectedFiles();

  fileButton->setText(QString::number(fileList.count()) + " Symbols");

  delete dialog;
}

void Scanner::allSymbolsToggled (bool d)
{
  if (d)
    fileButton->setEnabled(FALSE);
  else
    fileButton->setEnabled(TRUE);
}

void Scanner::slotHelp ()
{
  QString s = "workwithscanner.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}
