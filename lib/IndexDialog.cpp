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

#include "IndexDialog.h"
#include "SymbolDialog.h"
#include "PrefDialog.h"
#include "HelpWindow.h"
#include "SymbolButton.h"
#include "Config.h"
#include "DbPlugin.h"
#include <qlayout.h>
#include <qdir.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include "../pics/insert.xpm"
#include "../pics/edit.xpm"
#include "../pics/deleteitem.xpm"

IndexDialog::IndexDialog (QString &nam, QString &l) : Q3TabDialog (0, "IndexDialog", TRUE)
{
  helpFile = "indexes.html";

  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  Q3GridLayout *grid = new Q3GridLayout(vbox);
  grid->setColStretch(1, 1);
  grid->setSpacing(5);

  QLabel *label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 0, 0);

  name = new QLineEdit(nam, w);
  grid->addWidget(name, 0, 1);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(vbox);
  hbox->setSpacing(2);

  list = new Q3ListView(w);
  list->addColumn(tr("Symbol"), 200);
  list->addColumn(tr("Weight"), -1);
  QObject::connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  hbox->addWidget(list);

  toolbar = new Toolbar(w, Toolbar::Vertical);
  hbox->addWidget(toolbar);

  QString s = "add";
  QString s2 = tr("Add Item");
  toolbar->addButton(s, insert, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(addItem()));

  s = "edit";
  s2 = tr("Edit");
  toolbar->addButton(s, edit, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editItem()));

  s = "delete";
  s2 = tr("Delete");
  toolbar->addButton(s, deleteitem, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deleteItem()));

  vbox->insertStretch(-1, 1);

  addTab(w, tr("Details"));

  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));

  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  setList(l);

  resize(640, 480);
}

IndexDialog::IndexDialog () : Q3TabDialog (0, "IndexDialog", TRUE)
{
}

IndexDialog::~IndexDialog ()
{
}

void IndexDialog::buttonStatus ()
{
  QString s = "edit";
  QString s2 = "delete";

  Q3ListViewItem *item = list->selectedItem();
  if (! item)
  {
    toolbar->setButtonStatus(s, FALSE);
    toolbar->setButtonStatus(s2, FALSE);
  }
  else
  {
    toolbar->setButtonStatus(s, TRUE);
    toolbar->setButtonStatus(s2, TRUE);
  }
}

void IndexDialog::addItem ()
{
  double weight = 1;
  QString s;

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Add Index Item"));
  QString pl = tr("Details");
  dialog->createPage (pl);
  QString t = tr("Symbol");
  Config config;
  QString t2;
  config.getData(Config::DataPath, t2);
  dialog->addSymbolItem(t, pl, t2, s);
  t = tr("Weight");
  dialog->addDoubleItem(t, pl, weight);
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    t = tr("Symbol");
    dialog->getSymbol(t, s);
    if (! s.length())
    {
      delete dialog;
      return;
    }

    t = tr("Weight");
    weight = dialog->getDouble(t);

    QStringList l = QStringList::split("/", s, FALSE);
    symbolDict.insert(l[l.count() - 1], new QString(s));

    new Q3ListViewItem(list, l[l.count() - 1], QString::number(weight));

    buttonStatus();
  }

  delete dialog;
}

void IndexDialog::editItem ()
{
  Q3ListViewItem *item = list->selectedItem();
  if (! item)
    return;

  QString s = item->text(0);
  double weight = item->text(1).toFloat();

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Index Item"));
  QString pl = tr("Details");
  dialog->createPage (pl);
  QString t = tr("Symbol");
  Config config;
  QString t2;
  config.getData(Config::DataPath, t2);
  dialog->addSymbolItem(t, pl, t2, s);
  t = tr("Weight");
  dialog->addDoubleItem(t, pl, weight);
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    t = tr("Symbol");
    dialog->getSymbol(t, s);
    if (! s.length())
    {
      delete dialog;
      return;
    }

    t = tr("Weight");
    weight = dialog->getDouble(t);

    symbolDict.remove(item->text(0));
    QStringList l = QStringList::split("/", s, FALSE);
    symbolDict.insert(l[l.count() - 1], new QString(s));

    item->setText(0, l[l.count() - 1]);
    item->setText(1, QString::number(weight));

    buttonStatus();
  }

  delete dialog;
}

void IndexDialog::deleteItem ()
{
  Q3ListViewItem *item = list->selectedItem();
  if (! item)
    return;

  symbolDict.remove(item->text(0));
  delete item;

  buttonStatus();
}

void IndexDialog::setList (QString &d)
{
  list->clear();
  symbolDict.clear();

  QStringList l = QStringList::split(":", d, FALSE);

  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    QStringList l2 = QStringList::split("/", l[loop], FALSE);
    symbolDict.insert(l2[l2.count() - 1], new QString(l[loop]));
    new Q3ListViewItem(list, l2[l2.count() - 1], l[loop + 1]);
  }

  buttonStatus();
}

void IndexDialog::getList (QString &d)
{
  d.truncate(0);

  if (! list->childCount())
    return;

  Q3ListViewItem *item = list->firstChild();

  while (item)
  {
    QString *sp = symbolDict[item->text(0)];
    d.append(sp->left(sp->length()));
    d.append(":");
    d.append(item->text(1));
    d.append(":");

    item = item->nextSibling();
  }
}

void IndexDialog::getName (QString &d)
{
  d = name->text();
}

void IndexDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}
