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

#include "CUSDialog.h"
#include "HelpWindow.h"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3frame.h>
#include <qdir.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

CUSDialog::CUSDialog (QString d) : Q3TabDialog (0, "CUSDialog", TRUE)
{
  setCaption(tr("CUS Indicator"));

  helpFile = d;

  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  list = new FormulaEdit(w, FormulaEdit::Indicator);
  vbox->addWidget(list);

  addTab(w, tr("Settings"));

  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  resize(640, 480);
}

CUSDialog::~CUSDialog ()
{
}

void CUSDialog::setLine (QString d)
{
  list->setLine(d);
}

void CUSDialog::getList (QStringList &l)
{
  l.clear();
  QString s;
  list->getText(s);
  l = QStringList::split("\n", s, FALSE);
}

void CUSDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}
