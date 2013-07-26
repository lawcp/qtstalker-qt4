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

#include "SymbolDialog.h"
//Added by qt3to4:
#include <QKeyEvent>

SymbolDialog::SymbolDialog (QWidget *w,QString &bp,QString &dir,QString &filter,Q3FileDialog::Mode mode) :
                           Q3FileDialog (dir, filter, w, "SymbolDialog", TRUE)
{
  basePath = bp;
  setMode(mode);
  connect(this, SIGNAL(dirEntered(const QString &)), this, SLOT(dirSelected(const QString &)));
  keyFlag = FALSE;
}

SymbolDialog::~SymbolDialog ()
{
}

void SymbolDialog::dirSelected (const QString &d)
{
  if (mode() == Q3FileDialog::DirectoryOnly)
  {
    blockSignals(TRUE);
    setDir(basePath);
    blockSignals(FALSE);
    return;
  }

  if (d.length() < basePath.length())
  {
    blockSignals(TRUE);
    setDir(basePath);
    blockSignals(FALSE);
  }
}

void SymbolDialog::setKeyFlag (bool d)
{
  keyFlag = d;
}

void SymbolDialog::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (type, key->state(), key->key(), key->ascii(), key->text());

  doKeyPress(key);
}

void SymbolDialog::doKeyPress (QKeyEvent *key)
{
  key->accept();
  Q3FileDialog::keyPressEvent(key);
}

void SymbolDialog::setType (int d)
{
  type = d;
}
