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

#include "FontButton.h"
#include <qfontdialog.h>

FontButton::FontButton (QWidget *w, QFont &f) : QPushButton (w)
{
  font = f;
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fontDialog()));
  setMaximumHeight(25);
  setToggleButton(FALSE);
  setFontButton();
  setText(tr("Font"));
  changed = FALSE;
}

FontButton::~FontButton ()
{
}

void FontButton::setFontButton ()
{
  setFont(font);
}

void FontButton::getFont (QFont &d)
{
  d = font;
}

void FontButton::fontDialog ()
{
  bool ok;
  QFont f = QFontDialog::getFont(&ok, font, this);
  if (ok)
  {
    if (font != f)
    {
      font = f;
      setFontButton();
      changed = TRUE;
      emit valueChanged();
    }
  }
}

bool FontButton::isChanged()
{
  bool b = changed;
  changed = FALSE;
  return b;
}
