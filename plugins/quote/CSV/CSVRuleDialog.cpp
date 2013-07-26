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

#include "CSVRuleDialog.h"
#include "../../../pics/insert.xpm"
#include "../../../pics/deleteitem.xpm"
#include "Setting.h"
#include "HelpWindow.h"
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qinputdialog.h>
#include <qdir.h>
#include <qfile.h>
#include <q3textstream.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

CSVRuleDialog::CSVRuleDialog (QWidget *p, QString &d) : Q3TabDialog (p, "CSVRuleDialog", TRUE)
{
  helpFile = "csv.html";
  saveFlag = FALSE;
  rulePath = d;

  createRulePage();
  setOkButton();
  setApplyButton(tr("&Save"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveRule()));
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
  resize(640, 480);

  QFileInfo fi(rulePath);
  ruleName->setText(fi.fileName());

  QDir dir;
  if (dir.exists(rulePath))
    loadRule();

  QString s = tr("Editing CSV Rule: ") + ruleName->text();
  setCaption(s);
}

CSVRuleDialog::~CSVRuleDialog ()
{
}

void CSVRuleDialog::createRulePage ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 5, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("Rule:"), w);
  grid->addWidget(label, 0, 0);

  ruleName = new QLineEdit(w);
  grid->addWidget(ruleName, 0, 1);
  ruleName->setReadOnly(TRUE);

  label = new QLabel(tr("Chart Type:"), w);
  grid->addWidget(label, 1, 0);

  type = new QComboBox(w);
  type->insertItem("Stocks", -1);
  type->insertItem("Futures", -1);
  connect(type, SIGNAL(activated(int)), this, SLOT(comboChanged(int)));
  grid->addWidget(type, 1, 1);

  label = new QLabel(tr("Delimiter:"), w);
  grid->addWidget(label, 2, 0);

  delimiter = new QComboBox(w);
  delimiter->insertItem(tr("Comma"), -1);
  delimiter->insertItem(tr("Tab"), -1);
  delimiter->insertItem(tr("Space"), -1);
  delimiter->insertItem(tr("Semicolon"), -1);
  connect(type, SIGNAL(activated(int)), this, SLOT(comboChanged(int)));
  grid->addWidget(delimiter, 2, 1);

  label = new QLabel(tr("Data Directory:"), w);
  grid->addWidget(label, 3, 0);

  directory = new QLineEdit(w);
  connect(directory, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(directory, 3, 1);

  label = new QLabel(tr("Symbol Filter:"), w);
  grid->addWidget(label, 4, 0);

  symbolFilter = new QLineEdit(w);
  connect(symbolFilter, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(symbolFilter, 4, 1);

  vbox->addSpacing(10);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(vbox);
  hbox->setSpacing(5);

  fieldList = new Q3ListBox(w);
  fieldList->setSelectionMode(Q3ListBox::Extended);
  fieldList->insertItem("Symbol", -1);
  fieldList->insertItem("Date:YYYYMMDD", -1);
  fieldList->insertItem("Date:YYMMDD", -1);
  fieldList->insertItem("Date:MMDDYY", -1);
  fieldList->insertItem("Date:MMDDYYYY", -1);
  fieldList->insertItem("Date:DDMMYYYY", -1);
  fieldList->insertItem("Date:MMDDYYYYHHMMSS", -1);
  fieldList->insertItem("Date:YYYYMMDDHHMMSS", -1);
  fieldList->insertItem("Time", -1);
  fieldList->insertItem("Open", -1);
  fieldList->insertItem("High", -1);
  fieldList->insertItem("Low", -1);
  fieldList->insertItem("Close", -1);
  fieldList->insertItem("Volume", -1);
  fieldList->insertItem("OI", -1);
  fieldList->insertItem("Ignore", -1);
  fieldList->insertItem("Name", -1);
  QObject::connect(fieldList, SIGNAL(highlighted(int)), this, SLOT(fieldListSelected(int)));
  hbox->addWidget(fieldList);

  ruleToolbar = new Toolbar(w, Toolbar::Horizontal);
  hbox->addWidget(ruleToolbar);

  QString s = "insert";
  QString s2 = tr("Insert Field");
  ruleToolbar->addButton(s, insert, s2);
  QObject::connect(ruleToolbar->getButton(s), SIGNAL(clicked()), this, SLOT(insertField()));
  ruleToolbar->setButtonStatus(s, FALSE);

  s = "deleteitem";
  s2 = tr("Delete Field");
  ruleToolbar->addButton(s, deleteitem, s2);
  QObject::connect(ruleToolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deleteField()));
  ruleToolbar->setButtonStatus(s, FALSE);

  ruleList = new Q3ListBox(w);
  QObject::connect(ruleList, SIGNAL(highlighted(int)), this, SLOT(ruleFieldSelected(int)));
  hbox->addWidget(ruleList);

  addTab(w, tr("Rule"));
}

void CSVRuleDialog::saveRule ()
{
  if (! saveFlag)
    return;

  if (! directory->text().length())
  {
    QMessageBox::information(this, tr("Error"), tr("Must inlcude a directory."));
    return;
  }

  if (directory->text().contains(" "))
  {
    QMessageBox::information(this, tr("Error"), tr("No spaces allowed in directory name."));
    return;
  }

  if (! directory->text().right(1).compare("/") ||
      ! directory->text().left(1).compare("/"))
  {
    QMessageBox::information(this, tr("Error"), tr("Invalid directory name."));
    return;
  }

  QFile f(rulePath);
  if (! f.open(QIODevice::WriteOnly))
  {
    QMessageBox::information(this, tr("Disk Error"), tr("Cannot save file."));
    return;
  }
  Q3TextStream stream(&f);

  stream << "Delimiter=" << delimiter->currentText() << "\n";
  stream << "Type=" << type->currentText() << "\n";
  stream << "Directory=" << directory->text() << "\n";
  stream << "SymbolFilter=" << symbolFilter->text() << "\n";

  int loop;
  QStringList l;
  for (loop = 0; loop < (int) ruleList->count(); loop++)
    l.append(ruleList->text(loop));
  stream << "Rule=" << l.join(",") << "\n";

  f.close();

  saveFlag = FALSE;
}

void CSVRuleDialog::loadRule ()
{
  QFile f(rulePath);
  if (! f.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(this, tr("Disk Error"), tr("Cannot read file."));
    return;
  }
  Q3TextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    QStringList l = QStringList::split("=", s, FALSE);
    if (l.count() != 2)
      continue;

    if (! l[0].compare("Delimiter"))
    {
      delimiter->setCurrentText(l[1]);
      continue;
    }

    if (! l[0].compare("Type"))
    {
      type->setCurrentText(l[1]);
      continue;
    }

    if (! l[0].compare("Directory"))
    {
      directory->setText(l[1]);
      continue;
    }

    if (! l[0].compare("SymbolFilter"))
    {
      symbolFilter->setText(l[1]);
      continue;
    }

    if (! l[0].compare("Rule"))
    {
      QStringList l2 = QStringList::split(",", l[1], FALSE);
      ruleList->insertStringList(l2, -1);
    }
  }

  f.close();
}

void CSVRuleDialog::insertField ()
{
  int loop;
  for (loop = 0; loop < (int) fieldList->count(); loop++)
  {
    if (fieldList->isSelected(loop))
    {
      ruleList->insertItem(fieldList->text(loop), ruleList->currentItem());
      saveFlag = TRUE;
    }
  }
}

void CSVRuleDialog::deleteField ()
{
  if (ruleList->currentItem() != -1)
  {
    ruleList->removeItem(ruleList->currentItem());
    saveFlag = TRUE;
  }
}

void CSVRuleDialog::ruleFieldSelected (int)
{
  QString s = "deleteitem";
  if (ruleList->currentItem() != -1)
    ruleToolbar->setButtonStatus(s, TRUE);
  else
    ruleToolbar->setButtonStatus(s, FALSE);
}

void CSVRuleDialog::fieldListSelected (int d)
{
  QString s = "insert";
  if (d != -1)
    ruleToolbar->setButtonStatus(s, TRUE);
  else
    ruleToolbar->setButtonStatus(s, FALSE);
}

void CSVRuleDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void CSVRuleDialog::comboChanged (int)
{
  saveFlag = TRUE;
}

void CSVRuleDialog::textChanged (const QString &)
{
  saveFlag = TRUE;
}


