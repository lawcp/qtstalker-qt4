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

#include "FormulaEdit.h"
#include "PrefDialog.h"
#include "IndicatorPlugin.h"
#include "SymbolDialog.h"
#include "../pics/deleteitem.xpm"
#include "../pics/edit.xpm"
#include "../pics/insert.xpm"
#include "../pics/openchart.xpm"
#include "../pics/include.xpm"
#include <qlayout.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qinputdialog.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

// not used #define BUTTON_SIZE 24

FormulaEdit::FormulaEdit (QWidget *w, int t) : QWidget(w)
{
  type = (FormulaEditType) t;
  /*  FIXME: take no effect, outdated or not yet implemented?
  config.getPluginList(Config::IndicatorPluginPath, functionList);
  */

  PlotLine pl;
  pl.getLineTypes(lineTypes);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(1);

  QTabWidget *tab = new QTabWidget(this);
  hbox->addWidget(tab);

  // create formula page
  QWidget *tw = new QWidget(this);

  Q3HBoxLayout *thbox = new Q3HBoxLayout(tw);
  thbox->setMargin(5);
  thbox->setSpacing(0);

  formula = new Q3TextEdit(tw);
  thbox->addWidget(formula);

  ftoolbar = new Toolbar(tw, Toolbar::Vertical);
  thbox->addWidget(ftoolbar);

  QString s = "open";
  QString s2 = tr("Open Rule");
  ftoolbar->addButton(s, openchart, s2);
  QObject::connect(ftoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(openRule()));

  s = "include";
  s2 = tr("Include Rule");
  ftoolbar->addButton(s, include, s2);
  QObject::connect(ftoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(includeRule()));

  s = "add";
  s2 = tr("Function Dialog");
  ftoolbar->addButton(s, insert, s2);
  QObject::connect(ftoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(functionDialog()));

  tab->addTab(tw, tr("Formula"));

  // create plot page
  tw = new QWidget(this);

  thbox = new Q3HBoxLayout(tw);
  thbox->setMargin(5);
  thbox->setSpacing(0);

  plot = new Q3ListBox(tw);
  connect(plot, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(slotDoubleClicked(Q3ListBoxItem *)));
  thbox->addWidget(plot);

  ptoolbar = new Toolbar(tw, Toolbar::Vertical);
  thbox->addWidget(ptoolbar);

  s = "insert";
  s2 = tr("Insert");
  ptoolbar->addButton(s, insert, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(insertPlotItem()));

  s = "edit";
  s2 = tr("Edit");
  ptoolbar->addButton(s, edit, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editPlotItem()));

  s = "delete";
  s2 = tr("Delete");
  ptoolbar->addButton(s, deleteitem, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deletePlotItem()));

  tab->addTab(tw, tr("Plot"));
}

FormulaEdit::~FormulaEdit ()
{
}

void FormulaEdit::insertPlotItem ()
{
  QString pl = tr("Plot");
  QString cl = tr("Color");
  QString ll = tr("Label");
  QString ltl = tr("Line Type");
  QString vl = tr("Variable");

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Insert Plot"));
  dialog->createPage (pl);

  QString s("Var");
  QStringList l;
  getVariableList(l, FALSE);
  dialog->addComboItem(vl, pl, l, 0);

  QColor c("red");
  dialog->addColorItem(cl, pl, c);
  s = "Label";
  dialog->addTextItem(ll, pl, s);

  dialog->addComboItem(ltl, pl, lineTypes, 4);

  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  QString ts;
  s = "plot (";
  dialog->getCombo(vl, ts);
  s.append(ts + ",");
  dialog->getColor(cl, c);
  s.append(c.name() + ",");
  dialog->getText(ll, ts);
  if (! ts.length())
    ts = " ";
  s.append(ts + ",");
  dialog->getCombo(ltl, ts);
  s.append(ts + ")");
  plot->insertItem(s, plot->currentItem() + 1);

  delete dialog;
}

void FormulaEdit::editPlotItem ()
{
  QString s = plot->currentText();
  if (! s.length())
    return;
  s.remove(0, s.find("(", 0, TRUE) + 1);
  s.truncate(s.find(")", -1, TRUE));
  QStringList l = QStringList::split(",", s, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    l[loop] = l[loop].stripWhiteSpace();

  QString pl = tr("Plot");
  QString cl = tr("Color");
  QString ll = tr("Label");
  QString ltl = tr("Line Type");
  QString vl = tr("Variable");

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Edit Plot"));
  dialog->createPage (pl);

  s = "Var";
  QStringList l2;
  getVariableList(l2, FALSE);
  dialog->addComboItem(vl, pl, l2, l[0]);

  QColor c(l[1]);
  dialog->addColorItem(cl, pl, c);

  dialog->addTextItem(ll, pl, l[2]);

  l2 = lineTypes;
  dialog->addComboItem(ltl, pl, l2, l[3]);

  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  QString ts;
  s = "plot (";
  dialog->getCombo(vl, ts);
  s.append(ts + ",");
  dialog->getColor(cl, c);
  s.append(c.name() + ",");
  dialog->getText(ll, ts);
  if (! ts.length())
    ts = " ";
  s.append(ts + ",");
  dialog->getCombo(ltl, ts);
  s.append(ts + ")");
  plot->changeItem(s, plot->currentItem());

  delete dialog;
}

void FormulaEdit::deletePlotItem ()
{
  plot->removeItem(plot->currentItem());
}

void FormulaEdit::setLine (QString &d)
{
  if (d.contains("script="))
  {
    QStringList l = QStringList::split("=", d, FALSE);
    QString k = l[0];
    QString s = d;
    s.remove(0, k.length() + 1);
    QStringList l2 = QStringList::split("|", s, FALSE);
    int loop;
    for (loop = 0; loop < (int) l2.count(); loop++)
      setLine(l2[loop]);
    return;
  }

  if (d.contains(":="))
  {
    formula->append(d);
    return;
  }

  if (d.contains("//"))
  {
    formula->append(d);
    return;
  }

  if (d.contains("INCLUDECUS("))
  {
    formula->append(d);
    return;
  }

  if (d.contains("plot"))
    plot->insertItem(d, -1);
}

void FormulaEdit::getText (QString &s)
{
  s = formula->text()  + "\n";

  int loop;
  for (loop = 0; loop < (int) plot->count(); loop++)
    s.append(plot->text(loop) + "\n");
}

void FormulaEdit::openRule ()
{
  QString s("*");
  QString s2;
  config.getData(Config::IndicatorPath, s2);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s2,
  					  s2,
					  s,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select rule to open."));

  int rc = dialog->exec();

  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  QString selection = dialog->selectedFile();
  delete dialog;

  if (selection.isEmpty())
    return;

  QFile f(selection);
  if (! f.open(QIODevice::ReadOnly))
  {
    qDebug("FormulaEdit::openRule:can't read file %s", selection.latin1());
    return;
  }
  Q3TextStream stream(&f);

  QString script;
  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (s.contains("script="))
      script = s;
  }
  f.close();

  setLine(script);
}

/*
void FormulaEdit::saveRule ()
{
  if (! plot->count())
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("Plot missing."));
    return;
  }

  QString s("*");
  QString s2;
  config.getData(Config::IndicatorPath, s2);
  QString selection = QFileDialog::getSaveFileName(s2, s, this, "Save CUS Rule", "Save CUS Rule");
  if (! selection.length())
    return;

  while (selection.contains(" "))
    selection = selection.remove(selection.find(" ", 0, TRUE), 1);

  QDir dir;
  if (dir.exists(selection, TRUE))
  {
    int rc = QMessageBox::warning(this,
  			          tr("Qtstalker: Warning"),
			          tr("Rule already exists. Do you want to replace it?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::No)
      return;
  }

  QFile f(selection);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("FormulaEdit::saveItem:can't open file");
    return;
  }
  QTextStream stream(&f);

  int loop;
  for (loop = 0; loop < (int) formulaList.count(); loop++)
    stream << formulaList[loop] << "\n";
  getText(s);
  stream << s << "\n";

  f.close();
}
*/

void FormulaEdit::slotDoubleClicked (Q3ListBoxItem *)
{
  editPlotItem();
}

int FormulaEdit::getLines ()
{
  return (int) formula->lines();
}

void FormulaEdit::functionDialog ()
{
  Config config;
  QStringList l;
  config.getIndicatorList(l);
  l.append("UTIL");
  l.append("SYMBOL");
  l.sort();

  bool ok;
  QString function = QInputDialog::getItem(QObject::tr("Indicator Selection"),
                                    QObject::tr("Select an indicator:"),
                                    l,
                                    0,
                                    TRUE,
                                    &ok,
                                    this);
  if (! ok)
    return;

  IndicatorPlugin *plug = config.getIndicatorPlugin(function);
  if (! plug)
  {
    qDebug("FormulaEdit::functionDialog:can't open %s plugin", function.latin1());
    return;
  }

  QString s;
  plug->getPluginName(s);
  if (! s.compare("TALIB"))
    plug->setFormatMethod(function);

  QString vname, format;
  QStringList vl;
  getVariableList(vl, TRUE);
  plug->formatDialog(vl, vname, format);

  if (! vname.length())
    return;

  if (vl.findIndex(vname) != -1)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate variable name."));
    return;
  }

  format.prepend(vname + " := " + function + "(");
  format.append(")");
  formula->insert(format);
}

void FormulaEdit::includeRule ()
{
  QString s("*");
  QString s2, s3;
  config.getData(Config::IndicatorPath, s2);
  config.getData(Config::IndicatorGroup, s3);
  s2.append("/" + s3);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s2,
  					  s2,
					  s,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select rule to include"));

  int rc = dialog->exec();

  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  QStringList selection = dialog->selectedFiles();
  delete dialog;

  if (! selection.count())
    return;

  QFileInfo fi(selection[0]);
  s = "INCLUDECUS(" + fi.fileName() + ")\n";
  formula->insert(s);
}

void FormulaEdit::getVariableList (QStringList &l, bool flag)
{
  l.clear();

  if (flag)
  {
    QString s;
    BarData bd(s);
    bd.getInputFields(l);
  }

  QStringList l2 = QStringList::split("\n", formula->text(), FALSE);
  int loop;
  for (loop = 0; loop < (int) l2.count(); loop++)
  {
    if (l2[loop].contains(":="))
    {
      QStringList l3 = QStringList::split(":=", l2[loop], FALSE);
      l.append(l3[0].stripWhiteSpace());
    }
  }
}
