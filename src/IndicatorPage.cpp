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

#include "IndicatorPage.h"
#include "HelpWindow.h"
#include "Config.h"
#include "PrefDialog.h"
#include "SymbolDialog.h"
#include "IndicatorPlugin.h"
#include "DbPlugin.h"
#include "../pics/help.xpm"
#include "../pics/ok.xpm"
#include "../pics/ok_gray.xpm"
#include "../pics/disable.xpm"
#include "../pics/disable_gray.xpm"
#include "../pics/edit.xpm"
#include "../pics/deleteitem.xpm"
#include "../pics/newchart.xpm"
#include "../pics/moveitem.xpm"
#include "../pics/dirclosed.xpm"
#include <qcursor.h>
#include <q3accel.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QPixmap>
#include <QMenu>
#include <Q3VBoxLayout>


IndicatorPage::IndicatorPage (QWidget *w, DBIndex *i) : QWidget (w)
{
  chartIndex = i;
  updateEnableFlag = FALSE;
  idir.setFilter(QDir::Files);

  //Config config;
  rcfile.loadData(RcFile::IndicatorPath, baseDir);
  rcfile.loadData(RcFile::IndicatorGroup, currentGroup);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(5);

  group = new QComboBox(this);
  updateGroups();
  connect(group, SIGNAL(activated(int)), this, SLOT(slotGroupChanged(int)));
  QToolTip::add(group, tr("Indicator Groups"));
  vbox->addWidget(group);

  search = new QLineEdit(this);
  search->setText("*");
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  list = new Q3ListBox(this);
  connect(list, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(doubleClick(Q3ListBoxItem *)));
  connect(list, SIGNAL(contextMenuRequested(Q3ListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(Q3ListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(itemSelected(const QString &)));
  vbox->addWidget(list);

  menu = new QMenu(this);
  menu->insertItem(QPixmap(dirclosed), tr("&New Indicator Group		Ctrl+N"), this,
                   SLOT(newIndicatorGroup()));
  menu->insertItem(QPixmap(deleteitem), tr("Delete Indicator Group	Ctrl+X"), this,
                   SLOT(deleteIndicatorGroup()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(newchart), tr("Ne&w Indicator		Ctrl+W"), this,
                   SLOT(newIndicator()));
  menu->insertItem(QPixmap(newchart), tr("&Add Local Indicator		Ctrl+A"), this,
                   SLOT(addLocalIndicator()));
  menu->insertItem(QPixmap(edit), tr("&Edit Indicator			Ctrl+E"), this,
                   SLOT(editIndicator()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Indicator		Ctrl+D"), this,
                   SLOT(deleteIndicator()));
  menu->insertItem(QPixmap(moveitem), tr("Mo&ve Indicator		Ctrl+V"), this,
                   SLOT(moveIndicator()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help			Ctrl+H"), this, SLOT(slotHelp()));

  Q3Accel *a = new Q3Accel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_N, NewIndicatorGroup);
  a->insertItem(Qt::CTRL+Qt::Key_X, DeleteIndicatorGroup);
  a->insertItem(Qt::CTRL+Qt::Key_W, NewIndicator);
  a->insertItem(Qt::CTRL+Qt::Key_A, AddLocalIndicator);
  a->insertItem(Qt::CTRL+Qt::Key_D, DeleteIndicator);
  a->insertItem(Qt::CTRL+Qt::Key_E, EditIndicator);
  a->insertItem(Qt::CTRL+Qt::Key_V, MoveIndicator);
  a->insertItem(Qt::CTRL+Qt::Key_H, Help);
  a->insertItem(Qt::CTRL+Qt::Key_Tab, Tab);

  itemSelected(QString());
}

IndicatorPage::~IndicatorPage ()
{
}

void IndicatorPage::itemSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(5), TRUE);
    menu->setItemEnabled(menu->idAt(6), TRUE);
    menu->setItemEnabled(menu->idAt(7), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(5), FALSE);
    menu->setItemEnabled(menu->idAt(6), FALSE);
    menu->setItemEnabled(menu->idAt(7), FALSE);
  }
}

void IndicatorPage::newIndicatorGroup ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Indicator Group"),
  				    tr("Enter new group symbol."),
				    QLineEdit::Normal,
				    tr("NewGroup"),
				    &ok,
				    this);
  if ((! ok) || (s.isNull()))
    return;

  int loop;
  QString selection;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      selection.append(c);
  }

  s = baseDir + "/" + selection;
  QDir dir(s);
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  dir.mkdir(s, TRUE);
  updateGroups();
}

void IndicatorPage::deleteIndicatorGroup ()
{
  Config config;
  QString s("*");
  QString s2;
  rcfile.loadData(RcFile::IndicatorPath, s2);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s2,
  					  s2,
					  s,
					  Q3FileDialog::DirectoryOnly);
  dialog->setCaption(tr("Select Indicator Group(s) To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure to delete selected indicator groups?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QString l = dialog->selectedFile();
    if (! l.isEmpty())
    {
      delete dialog;
      return;
    }

    QDir dir(l);
    bool flag = FALSE;
    if (! dir.dirName().compare(currentGroup))
      flag = TRUE;

    if (! dir.dirName().compare("Indicators"))
    {
      delete dialog;
      QMessageBox::information(this,
                               tr("Qtstalker: Delete Indicator Group"),
                               tr("Cannot delete default group."));
      return;
    }
    else
    {
      int loop;
      for (loop = 2; loop < (int) dir.count(); loop++)
      {
        QString s = dir.absPath() + "/" + dir[loop];
        if (! dir.remove(s))
          qDebug("IndicatorPage::deleteGroupItem:failed to delete file");
      }

      if (! dir.rmdir(dir.absPath(), TRUE))
        qDebug("IndicatorPage::deleteGroupItem:failed to delete dir");
    }

    updateGroups();

    if (flag)
      slotGroupChanged(0);

    itemSelected(QString());
  }

  delete dialog;
}

void IndicatorPage::newIndicator ()
{
  Config config;
  PrefDialog *idialog = new PrefDialog;
  idialog->setCaption(tr("New Indicator"));
  QString s("newindicator.html");
  idialog->setHelpFile(s);

  QString pl = tr("Details");
  QString il = tr("Indicator");
  QString nl = tr("Name");
  QString ptl = tr("Tab Row");
  QString gl = tr("Group");

  idialog->createPage (pl);

  QStringList l2;
  config.getIndicatorList(l2);
  rcfile.loadData(RcFile::LastNewIndicator, s);
  idialog->addComboItem(il, pl, l2, l2.findIndex(s));

  s = tr("NewIndicator");
  idialog->addTextItem(nl, pl, s);

  idialog->addIntItem(ptl, pl, 1, 1, 3);

  QStringList l3;
  int loop;
  for (loop = 0; loop < group->count(); loop++)
    l3.append(group->text(loop));
  idialog->addComboItem(gl, pl, l3, group->currentItem());

  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }

  QString name;
  idialog->getText(nl, name);
  QString indicator;
  idialog->getCombo(il, indicator);
  rcfile.saveData(RcFile::LastNewIndicator, indicator);
  int tabRow = idialog->getInt(ptl);
  QString tgroup;
  idialog->getCombo(gl, tgroup);
  delete idialog;

  if (! name.length())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator name missing."));
    return;
  }

  // correct any forbidden chars in name
  s.truncate(0);
  for (loop = 0; loop < (int) name.length(); loop++)
  {
    QChar c = name.at(loop);
    if (c.isLetterOrNumber())
      s.append(c);
  }
  name = s;

  // check if we can save this indicator in the selected group
  QDir dir;
  s = baseDir + "/" + tgroup + "/" + name;
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  // get the indicator plugin
  IndicatorPlugin *plug = config.getIndicatorPlugin(indicator);
  if (! plug)
  {
    qDebug("IndicatorPage::newIndicator - could not open plugin");
    config.closePlugin(indicator);
    return;
  }

  QString ts;
  plug->getPluginName(ts);
  if (! ts.compare("TALIB"))
  {
    Setting a;
    ts = "method";
    a.setData(ts, indicator);
    plug->setIndicatorSettings(a);
  }
  rc = plug->indicatorPrefDialog(this);
  if (! rc)
  {
    config.closePlugin(indicator);
    return;
  }

  // get the indicator settings
  Setting tset;
  plug->getIndicatorSettings(tset);
  ts = "Name";
  tset.setData(ts, name);
  ts = "enable";
  QString ts2 = "1";
  tset.setData(ts, ts2);
  ts = "tabRow";
  ts2 = QString::number(tabRow);
  tset.setData(ts, ts2);
  ts = "logScale";
  ts2 = "0";
  tset.setData(ts, ts2);
  ts = "dateFlag";
  ts2 = "1";
  tset.setData(ts, ts2);

  config.setIndicator(s, tset);
  config.closePlugin(indicator);

  // check if we need to load this indicator now
  if (tgroup.compare(currentGroup))
    return;

  Indicator *i = new Indicator;
  i->setName(name);
  i->setTabRow(tabRow);
  i->setFile(s);
  i->setType(indicator);
  emit signalNewIndicator(i);
}

void IndicatorPage::addLocalIndicator ()
{
  Config config;
  PrefDialog *idialog = new PrefDialog;
  idialog->setCaption(tr("Add Local Indicator"));
  QString s("addlocalindicator.html");
  idialog->setHelpFile(s);

  QString pl = tr("Details");
  QString il = tr("Indicator");

  idialog->createPage (pl);

  QStringList l;
  rcfile.loadData(RcFile::IndicatorPath, s);
  idialog->addFileItem(il, pl, l, s);

  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }

  idialog->getFile(il, l);
  delete idialog;

  if (! l.count())
    return;

  DBIndex index;
  rcfile.loadData(RcFile::IndexPath, s);
  index.open(s);
  QFileInfo fi(chartPath);
  s = fi.fileName();
  index.addIndicator(s, l[0]);

  Setting set;
  config.getIndicator(l[0], set);
  if (! set.count())
  {
    qDebug("IndicatorPage::editIndicator:indicator settings empty");
    return;
  }

  Indicator *i = new Indicator;
  i->setIndicator(set, l[0]);
  emit signalNewIndicator(i);
  updateList();
}

void IndicatorPage::editIndicator ()
{
  QString s = list->currentText();
  editIndicator(s);
}

void IndicatorPage::editIndicator (QString d)
{
  Config config;
  QDir dir;
  QString s = baseDir + "/" + currentGroup + "/" + d;
  if (! dir.exists(s))
  {
    qDebug("IndicatorPage::editIndicator: indicator not found %s", s.latin1());
    return;
  }

  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("IndicatorPage::editIndicator:indicator settings empty");
    return;
  }

  QString ts = "plugin";
  QString type;
  set.getData(ts, type);

  IndicatorPlugin *plug = config.getIndicatorPlugin(type);
  if (! plug)
  {
    qDebug("IndicatorPage::editIndicator - could not open plugin");
    config.closePlugin(type);
    return;
  }

  plug->setIndicatorSettings(set);

  int rc = plug->indicatorPrefDialog(this);
  if (! rc)
  {
    config.closePlugin(type);
    return;
  }

  plug->getIndicatorSettings(set);
  config.setIndicator(s, set);
  config.closePlugin(type);

  Indicator *i = new Indicator;
  i->setIndicator(set, s);
  if (i->getEnable())
    emit signalEditIndicator(i);
  else
    delete i;
}

void IndicatorPage::deleteIndicator ()
{
  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure to delete this indicator?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QDir dir;
  Config config;
  QString s;
  rcfile.loadData(RcFile::IndicatorPath, s);
  s.append("/" + currentGroup + "/" + list->currentText());
  if (! dir.exists(s))
  {
    qDebug("IndicatorPage::deleteIndicator: indicator not found %s", s.latin1());
    return;
  }

  QFileInfo fi(s);

  if (chartPath.length())
  {
    QString fn;
    QStringList l;
    QFileInfo fi2(chartPath);
    fn = fi2.fileName();
    chartIndex->getChartObjects(fn, l);
    int loop;
    Setting set;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      set.parse(l[loop]);
      QString ts = "Plot";
      QString ts2;
      set.getData(ts, ts2);
      if (! ts2.compare(list->currentText()))
      {
        ts = "Name";
        set.getData(ts, s);
        chartIndex->deleteChartObject(fn, s);
      }
    }

    if (fi.isSymLink())
    {
      QString ts = fi.readLink();
      chartIndex->deleteIndicator(fn, ts);
    }
  }

  itemSelected(QString());
  emit signalDeleteIndicator(list->currentText());
}

void IndicatorPage::moveIndicator ()
{
  Config config;
  QDir dir;
  QString s = baseDir + "/" + currentGroup + "/" + list->currentText();
  if (! dir.exists(s))
  {
    qDebug("IndicatorPage::moveIndicator: indicator not found %s", s.latin1());
    return;
  }

  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("IndicatorPage::moveIndicator:indicator settings empty");
    return;
  }

  QString pl = tr("Details");
  QString trl = tr("Tab Row");
  QString igl = tr("Indicator Group");

  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Move Indicator"));
  QString t("workwithmainindicators.html");
  dialog->setHelpFile(t);
  dialog->createPage(pl);

  QString ts = "tabRow";
  dialog->addIntItem(trl, pl, set.getInt(ts), 1, 3);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  ts = "tabRow";
  QString ts2 = QString::number(dialog->getInt(trl));
  set.setData(ts, ts2);

  ts = "enable";
  bool status = set.getInt(ts);
  if (status)
    emit signalDisableIndicator(s);

  config.setIndicator(s, set);

  if (status)
    emit signalEnableIndicator(s);

  updateList();
}

void IndicatorPage::updateList ()
{
  list->clear();

  Config config;
  QString ts;
  rcfile.loadData(RcFile::IndicatorPath, ts);
  idir.setPath(ts + "/" + currentGroup);

  int loop;
  for (loop = 0; loop < (int) idir.count(); loop++)
  {
    QString path = idir.absPath() + "/" + idir[loop];
    QFileInfo fi(path);
    QString s = idir[loop];
    Setting set;
    config.getIndicator(path, set);
    QString ts = "enable";
    if (! set.getInt(ts))
    {
      if (fi.isSymLink())
        list->insertItem(disable_gray, s, -1);
      else
        list->insertItem(disable, s, -1);
    }
    else
    {
      if (fi.isSymLink())
        list->insertItem(ok_gray, s, -1);
      else
        list->insertItem(ok, s, -1);

      if (updateEnableFlag)
        emit signalEnableIndicator(path);
    }
  }

  itemSelected(QString());
}

void IndicatorPage::doubleClick (Q3ListBoxItem *item)
{
  if (! item)
    return;

  QString s = item->text();
  changeIndicator(s);
}

void IndicatorPage::changeIndicator (QString &d)
{
  if (! d.length())
    return;

  QDir dir;
  QString s = baseDir + "/" + currentGroup + "/" + d;
  if (! dir.exists(s))
  {
    qDebug("IndicatorPage::changeIndicator: indicator not found %s", s.latin1());
    return;
  }

  QFileInfo fi(s);
  Config config;
  Setting set;
  config.getIndicator(s, set);
  QString ts = "enable";
  QString ts2;
  int t = set.getInt(ts);
  if (t)
  {
    ts2 = "0";
    set.setData(ts, ts2);
    config.setIndicator(s, set);
    if (fi.isSymLink())
      list->changeItem(disable_gray, d, list->currentItem());
    else
      list->changeItem(disable, d, list->currentItem());
    emit signalDisableIndicator(s);
  }
  else
  {
    ts2 = "1";
    set.setData(ts, ts2);
    config.setIndicator(s, set);
    if (fi.isSymLink())
      list->changeItem(ok_gray, d, list->currentItem());
    else
      list->changeItem(ok, d, list->currentItem());
    emit signalEnableIndicator(s);
  }
}

void IndicatorPage::slotHelp ()
{
  QString s = "workwithmainindicators.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void IndicatorPage::rightClick (Q3ListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void IndicatorPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);
}

void IndicatorPage::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        slotAccel(NewIndicatorGroup);
	break;
      case Qt::Key_X:
        slotAccel(DeleteIndicatorGroup);
	break;
      case Qt::Key_W:
        slotAccel(NewIndicator);
	break;
      case Qt::Key_A:
        slotAccel(AddLocalIndicator);
        break;
      case Qt::Key_D:
        slotAccel(DeleteIndicator);
	break;
      case Qt::Key_E:
        slotAccel(EditIndicator);
	break;
      case Qt::Key_V:
        slotAccel(MoveIndicator);
	break;
      case Qt::Key_Tab:
        slotAccel(Tab);
	break;
      default:
        break;
    }
  }
//  else
//  {
//    if (list->hasFocus())
//      list->keyPressEvent(key);
//    else
//      group->keyPressEvent(key);
//  }
}

void IndicatorPage::slotAccel (int id)
{
  switch (id)
  {
    case NewIndicatorGroup:
      newIndicatorGroup();
      break;
    case DeleteIndicatorGroup:
      deleteIndicatorGroup();
      break;
    case NewIndicator:
      newIndicator();
      break;
    case AddLocalIndicator:
      addLocalIndicator();
      break;
    case DeleteIndicator:
      deleteIndicator();
      break;
    case EditIndicator:
      editIndicator();
      break;
    case MoveIndicator:
      moveIndicator();
      break;
    case Help:
      slotHelp();
      break;
    case Tab:
      if (list->hasFocus())
        list->setFocus();
      else
        group->setFocus();
      break;
    default:
      break;
  }
}

void IndicatorPage::slotGroupChanged (int)
{
  if (group->count() == 1 && ! group->currentText().compare(currentGroup))
    return;

  emit signalGroupIsChanging();

  int loop;
  for (loop = 0; loop < (int) list->count(); loop++)
  {
    QString s = baseDir + "/" + currentGroup + "/" + list->text(loop);
    emit signalDisableIndicator(s);
  }

  currentGroup = group->currentText();
  rcfile.saveData(RcFile::IndicatorGroup, currentGroup);

  updateEnableFlag = TRUE;
  updateList();
  updateEnableFlag = FALSE;
  emit signalGroupChanged();
}

void IndicatorPage::updateGroups ()
{
  group->blockSignals(TRUE);

  group->clear();

  QStringList l;
  QDir dir(baseDir);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (fi.isDir())
      l.append(dir[loop]);
  }
  group->insertStringList(l, -1);

  group->setCurrentItem(l.findIndex(currentGroup));

  group->blockSignals(FALSE);
}

QString IndicatorPage::getIndicatorGroup ()
{
  return currentGroup;
}

void IndicatorPage::setFocus ()
{
  list->setFocus();
}

void IndicatorPage::setChartPath (QString &d)
{
  chartPath = d;
}

void IndicatorPage::addLocalIndicators (QString &d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  int loop;
  Setting set;
  Config config;
  QString s, ts, ts2;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s2 = baseDir + "/" + currentGroup + "/Local" + QString::number(loop + 1);
    QString s = "ln -s " + l[loop] + " " + s2;
    system(s);
    emit signalLocalIndicator(s2);
  }
}

void IndicatorPage::removeLocalIndicators ()
{
  QString s = baseDir + "/" + currentGroup;
  QDir dir(s);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = baseDir + "/" + currentGroup + "/" + dir[loop];
    QFileInfo fi(s);
    if (fi.isSymLink())
      emit signalDisableIndicator(s);
  }

  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = baseDir + "/" + currentGroup + "/" + dir[loop];
    QFileInfo fi(s);
    if (fi.isSymLink())
      dir.remove(s);
  }
}

void IndicatorPage::searchChanged (const QString &d)
{
  idir.setNameFilter(d);
  updateList();
}
