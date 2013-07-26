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

#include "TestPage.h"
#include "HelpWindow.h"
#include "../pics/help.xpm"
#include "../pics/openitem.xpm"
#include "../pics/newchart.xpm"
#include "../pics/deleteitem.xpm"
#include "../pics/renameitem.xpm"
#include "../pics/copyitem.xpm"
#include "SymbolDialog.h"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include <Q3VBoxLayout>
#include <stdlib.h>
#include <q3accel.h>
#include <qlayout.h>
#include <qtooltip.h>

TestPage::TestPage (QWidget *w, MainMenubar *mb, DBIndex *i) : QWidget (w)
{
  chartIndex = i;
  menubar = mb;
  idir.setFilter(QDir::Dirs);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(5);

  search = new QLineEdit(this);
  search->setText("*");
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  list = new Q3ListBox(this);
  connect(list, SIGNAL(contextMenuRequested(Q3ListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(Q3ListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(testSelected(const QString &)));
  connect(list, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(doubleClick(Q3ListBoxItem *)));
  vbox->addWidget(list);

  menu = new QMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Rule		Ctrl+N"), this, SLOT(newTest()));
  menu->insertItem(QPixmap(openitem), tr("&Open Rule		Ctrl+O"), this, SLOT(openTest()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Rule	Ctrl+D"), this, SLOT(deleteTest()));
  menu->insertItem(QPixmap(renameitem), tr("&Rename Rule		Ctrl+R"), this, SLOT(renameTest()));
  menu->insertItem(QPixmap(copyitem), tr("&Copy Rule		Ctrl+Y"), this, SLOT(copyTest()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  Q3Accel *a = new Q3Accel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_N, NewTest);
  a->insertItem(Qt::CTRL+Qt::Key_O, OpenTest);
  a->insertItem(Qt::CTRL+Qt::Key_D, DeleteTest);
  a->insertItem(Qt::CTRL+Qt::Key_R, RenameTest);
  a->insertItem(Qt::CTRL+Qt::Key_Y, CopyTest);
  a->insertItem(Qt::CTRL+Qt::Key_H, Help);

  updateList();
  testNoSelection();
}

TestPage::~TestPage ()
{
}

void TestPage::openTest ()
{
  Tester *dialog = new Tester(list->currentText(), chartIndex);
//  connect(menubar, SIGNAL(signalScale(bool)), dialog, SLOT(slotScaleToScreen(bool)));
  dialog->show();
}

void TestPage::newTest()
{
  Tester *dialog = new Tester;
  QString name = dialog->newTest();
  delete dialog;

  if (! name.length())
    return;

  updateList();

  dialog = new Tester(name, chartIndex);
//  connect(menubar, SIGNAL(signalScale(bool)), dialog, SLOT(slotScaleToScreen(bool)));
  dialog->show();
}

void TestPage::deleteTest()
{
  QString s("*");
  QString s2;
  config.getData(Config::TestPath, s2);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s2,
  					  s2,
					  s,
					  Q3FileDialog::DirectoryOnly);
  dialog->setCaption(tr("Select Backtest rules To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure to delete selected backtest rules?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    s = "rm -r " + dialog->selectedFile();

    if (system(s.latin1()) == -1)
      qDebug("TestPage::deleteTest:command failed");

    updateList();

    testNoSelection();
  }

  delete dialog;
}

void TestPage::renameTest ()
{
  if (list->currentItem() == -1)
    return;

  bool ok;
  QString s = QInputDialog::getText(tr("Rename Backtest Rule"),
  				    tr("Enter new backtest rule name."),
  				    QLineEdit::Normal,
				    list->currentText(),
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

  config.getData(Config::TestPath, s);
  s.append("/" + selection);
  QDir dir(s);
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  QString s2;
  config.getData(Config::TestPath, s2);
  s2.append("/" + list->currentText());
  dir.rename(s2, s);

  list->changeItem(selection, list->currentItem());
}

void TestPage::copyTest ()
{
  if (list->currentItem() == -1)
    return;

  bool ok;
  QString s = QInputDialog::getText(tr("Copy Backtest Rule"),
  				    tr("Enter new name of copy."),
  				    QLineEdit::Normal,
				    list->currentText(),
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

  config.getData(Config::TestPath, s);
  s.append("/" + selection);
  QDir dir(s);
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  QString ts;
  config.getData(Config::TestPath, ts);
  s = "cp -R " + ts + "/" + list->currentText() + " " + ts + "/" + selection;

  if (system(s.latin1()) == -1)
    qDebug("TestPage::copyTest:command failed");

  updateList();
  testNoSelection();
}

void TestPage::testSelected (const QString &)
{
  menu->setItemEnabled(menu->idAt(1), TRUE);
  menu->setItemEnabled(menu->idAt(2), TRUE);
  menu->setItemEnabled(menu->idAt(3), TRUE);
  menu->setItemEnabled(menu->idAt(4), TRUE);
}

void TestPage::testNoSelection ()
{
  menu->setItemEnabled(menu->idAt(1), FALSE);
  menu->setItemEnabled(menu->idAt(2), FALSE);
  menu->setItemEnabled(menu->idAt(3), FALSE);
  menu->setItemEnabled(menu->idAt(4), FALSE);
}

void TestPage::rightClick (Q3ListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void TestPage::updateList ()
{
  list->clear();

  QString s;
  config.getData(Config::TestPath, s);
  idir.setPath(s);
  int loop;
  for (loop = 0; loop < (int) idir.count(); loop++)
  {
    if (! idir[loop].compare(".") || ! idir[loop].compare(".."))
      ;
    else
      list->insertItem(idir[loop], -1);
  }
}

void TestPage::slotMessage (QString d)
{
  emit message(d);
}

void TestPage::doubleClick (Q3ListBoxItem *item)
{
  if (! item)
    return;

  Tester *dialog = new Tester(item->text(), chartIndex);
  dialog->show();
}

void TestPage::slotHelp ()
{
  QString s = "workwithbacktest.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void TestPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);
}

void TestPage::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_N:
        slotAccel(NewTest);
	break;
      case Qt::Key_D:
        slotAccel(DeleteTest);
	break;
      case Qt::Key_O:
        slotAccel(OpenTest);
	break;
      case Qt::Key_R:
        slotAccel(RenameTest);
	break;
      case Qt::Key_Y:
        slotAccel(CopyTest);
	break;
      default:
        break;
    }
  }
  else
  {
    switch (key->key())
    {
      case Qt::Key_Delete:
        deleteTest();
        break;
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        break;
      case Qt::Key_Enter:
      case Qt::Key_Return:
        openTest();
        break;
      default:
//        QListBox::keyPressEvent(key);
        break;
    }
  }
}

void TestPage::slotAccel (int id)
{
  switch (id)
  {
    case NewTest:
      newTest();
      break;
    case DeleteTest:
      deleteTest();
      break;
    case RenameTest:
      renameTest();
      break;
    case OpenTest:
      openTest();
      break;
    case CopyTest:
      copyTest();
      break;
    case Help:
      slotHelp();
      break;
    default:
      break;
  }
}

void TestPage::searchChanged (const QString &d)
{
  idir.setNameFilter(d);
  updateList();
}
