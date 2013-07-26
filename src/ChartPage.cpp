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

#include "ChartPage.h"
#include "Setting.h"
#include "SymbolDialog.h"
#include "DbPlugin.h"
#include "HelpWindow.h"
#include "DBIndex.h"
#include "../pics/addgroup.xpm"
#include "../pics/deleteitem.xpm"
#include "../pics/edit.xpm"
#include "../pics/exportfile.xpm"
#include "../pics/help.xpm"
#include "../pics/newchart.xpm"
#include "../pics/search.xpm"
#include <qmessagebox.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <q3accel.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include <Q3VBoxLayout>


ChartPage::ChartPage(QWidget *w, DBIndex *i) : QWidget(w)
{
  chartIndex = i;

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(5);

  search = new QLineEdit(this);
  search->setText("*");
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  QString s;
  rcfile.loadData(RcFile::DataPath, s);
  nav = new Navigator(this, s);
  connect(nav, SIGNAL(fileOpened(QString)), this, SLOT(chartOpened(QString)));
  connect(nav, SIGNAL(contextMenuRequested(Q3ListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(Q3ListBoxItem *)));
  nav->updateList();
  vbox->addWidget(nav);

  newMenu = new QMenu(this);
  int id = newMenu->insertItem(QPixmap(newchart), "&CC", this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), "&Index", this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), "&Spread", this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);

  menu = new QMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New..."), newMenu);
  menu->insertItem(QPixmap(edit), tr("&Edit Chart	Ctrl+E"), this, SLOT(editChart()));
//  menu->insertItem(QPixmap(search), tr("&Browse Chart	Ctrl+B"), this, SLOT(browseChart()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Chart	Ctrl+D"), this, SLOT(deleteChart()));
  menu->insertItem(QPixmap(exportfile), tr("E&xport Chart CSV	Ctrl+X"), this, SLOT(exportSymbol()));
  menu->insertItem(QPixmap(exportfile), tr("D&ump Chart		Ctrl+U"), this, SLOT(dumpSymbol()));
  menu->insertItem(QPixmap(addgroup), tr("Add To &Group	Ctrl+G"), this, SLOT(addToGroup()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help	Ctrl+H"), this, SLOT(slotHelp()));

  Q3Accel *a = new Q3Accel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_E, EditChart);
//  a->insertItem(Qt::CTRL+Qt::Key_B, BrowseChart);
  a->insertItem(Qt::CTRL+Qt::Key_D, DeleteChart);
  a->insertItem(Qt::CTRL+Qt::Key_X, ExportSymbol);
  a->insertItem(Qt::CTRL+Qt::Key_U, DumpSymbol);
  a->insertItem(Qt::CTRL+Qt::Key_H, Help);
  a->insertItem(Qt::CTRL+Qt::Key_Tab, Tab);
  a->insertItem(Qt::Key_Insert, BrowseChart);
  a->insertItem(Qt::Key_Delete, DeleteChartQuick);
  a->insertItem(Qt::CTRL+Qt::Key_G, AddToGroup);

  rcfile.loadData(RcFile::LastChartDir, s);
  if (!s.isEmpty())
    nav->setDirectory(s);
  else
    nav->updateList();
}

ChartPage::~ChartPage()
{
  QString s;
  nav->getCurrentPath(s);
  rcfile.saveData(RcFile::LastChartDir,s);
}

void ChartPage::browseChart()
{
  if (! nav->isSelected())
    return;

  QString s, u = "x-www-browser http://finance.yahoo.com/q?s=";
  nav->getFileSelection(s);
  if (! s.length())
    return;

  QRegExp rsym("(.*/Yahoo/.*/)(.*)");
  int pos = rsym.indexIn(s);
  if (pos > -1) {
      QString sym = rsym.cap(2); // Symbol
      u.append(sym + "&");
      system(u);
  }
}

void ChartPage::deleteChart()
{
  QString s("*");
  QString s2, bp;
  nav->getCurrentPath(s2);
  rcfile.loadData(RcFile::DataPath, bp);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          bp,
  					  s2,
					  s,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Charts To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
			      tr("Qtstalker: Warning"),
			      tr("Are you sure to delete selected charts?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      dir.remove(l[loop]);
      chartIndex->deleteChart(l[loop]);
    }

    nav->updateList();

    if (l.count())
      emit removeRecentCharts(l);
  }

  delete dialog;
}

void ChartPage::editChart()
{
  QString symbol;
  nav->getFileSelection(symbol);
  if (! symbol.length())
    return;

  QFileInfo fi(symbol);
  if (! fi.isFile())
    return;

  editChart(symbol);
}

void ChartPage::editChart(QString symbol)
{
  if (! symbol.length())
    return;

  QFileInfo fi(symbol);
  if (! fi.isFile())
    return;
  QString fn = fi.fileName();

  DbPlugin db;
  db.open(symbol, chartIndex);
  int rc = db.dbPrefDialog();
  db.close();
  if (rc)
    emit signalReloadChart();
}

void ChartPage::exportSymbol ()
{
  QString s("*");
  QString s2, bp;
  nav->getCurrentPath(s2);
  rcfile.loadData(RcFile::DataPath, bp);
  SymbolDialog *dialog = new SymbolDialog(this,
					  bp,
					  s2,
					  s,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Charts"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString s;
    rcfile.loadData(RcFile::Home, s);
    s.append("/export");
    QDir dir(s);
    if (! dir.exists(s))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("Unable to create export directory.");
	delete dialog;
        return;
      }
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
      exportChart(l[loop], TRUE);
  }

  delete dialog;
}

void ChartPage::dumpSymbol()
{
  QString s("*");
  QString s2, bp;
  nav->getCurrentPath(s2);
  rcfile.loadData(RcFile::DataPath, bp);
  SymbolDialog *dialog = new SymbolDialog(this,
					  bp,
					  s2,
					  s,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Charts"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rcfile.loadData(RcFile::Home, s);
    s.append("/export");
    QDir dir(s);
    if (! dir.exists(s))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("Unable to create export directory.");
	delete dialog;
        return;
      }
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
      exportChart(l[loop], FALSE);
  }

  delete dialog;
}

void ChartPage::exportChart(QString &path, bool f)
{
  DbPlugin db;
  db.open(path, chartIndex);

  QString s;
  rcfile.loadData(RcFile::Home, s);
  s.append("/export/");

  QString s2;
  QFileInfo fi(path);
  s2 = fi.fileName();
  if (! s2.length())
  {
    QStringList l = QStringList::split("/", path, FALSE);
    s.append(l[l.count() - 1]);
  }
  else
    s.append(s2);

  db.dump(s, f);

  db.close();
}

void ChartPage::chartOpened(QString d)
{
  emit fileSelected(d);
  emit addRecentChart(d);
}

void ChartPage::rightClick(Q3ListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void ChartPage::refreshList()
{
  nav->updateList();
}

void ChartPage::searchChanged(const QString &d)
{
  QString s(d);
  nav->setFilter(s);
}

void ChartPage::newChart(int id)
{
  QString type = newMenu->text(id);
  type.remove(type.find("&", 0, TRUE), 1);

  DbPlugin db;
  QString s;
  if (! type.compare("CC"))
    db.createNewCC(chartIndex);
  else
  {
    if (! type.compare("Index"))
      db.createNewIndex(chartIndex);
    else
      db.createNewSpread(chartIndex);
  }
  db.close();

  refreshList();
}

void ChartPage::slotHelp()
{
  QString s = "workwithcharts.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void ChartPage::setFocus()
{
  nav->setFocus();
}

void ChartPage::deleteChartQuick ()
{
  if (! nav->isSelected())
    return;

  QString s;
  nav->getFileSelection(s);
  if (! s.length())
    return;

  int rc = QMessageBox::warning(this,
			    tr("Qtstalker: Warning"),
			    tr("Are you sure to delete selected chart?"),
			    QMessageBox::Yes,
			    QMessageBox::No,
			    QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  QDir dir;
  dir.remove(s);
  chartIndex->deleteChart(s);
  nav->updateList();
}

void ChartPage::addToGroup()
{
  QString symbol;
  nav->getFileSelection(symbol);
  if (! symbol.length())
    return;

  QFileInfo fi(symbol);
  if (! fi.isFile())
    return;

  emit signalAddToGroup(symbol);
}

void ChartPage::doKeyPress(QKeyEvent *key)
{
  key->accept();

  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
//      case Qt::Key_B:
//        slotAccel(BrowseChart);
//	break;
      case Qt::Key_D:
        slotAccel(DeleteChart);
	break;
      case Qt::Key_E:
        slotAccel(EditChart);
	break;
      case Qt::Key_X:
        slotAccel(ExportSymbol);
	break;
      case Qt::Key_U:
        slotAccel(DumpSymbol);
	break;
      case Qt::Key_Tab:
        slotAccel(Tab);
	break;
      case Qt::Key_G:
        slotAccel(AddToGroup);
	break;
      default:
        break;
    }
  }
  else
  {
    switch(key->key())
    {
      case Qt::Key_Insert:
        browseChart();
	break;
      case Qt::Key_Delete:
        deleteChartQuick();
	break;
      default:
        if (! search->hasFocus())
          nav->doKeyPress(key);
        break;
    }
  }
}

void ChartPage::slotAccel(int id)
{
  switch (id)
  {
    case BrowseChart:
      browseChart();
      break;
    case DeleteChartQuick:
      deleteChartQuick();
      break;
    case DeleteChart:
      deleteChart();
      break;
    case EditChart:
      editChart();
      break;
    case ExportSymbol:
      exportSymbol();
      break;
    case DumpSymbol:
      dumpSymbol();
      break;
    case Help:
      slotHelp();
      break;
    case Tab:
      if (search->hasFocus())
        nav->setFocus();
      else
        search->setFocus();
      break;
    case AddToGroup:
      addToGroup();
      break;
    default:
      break;
  }
}

void ChartPage::setChartNavItem(QString chartDir, QString chartName)
{
  nav->setDirectory(chartDir);
  search->setText("*"); // perhaps they have recently used the filter, so show all
  nav->setNavItem(chartName);
}
