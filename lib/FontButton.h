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

#ifndef FONTBUTTON_HPP
#define FONTBUTTON_HPP

#include <qwidget.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qfont.h>

class FontButton : public QPushButton
{
  Q_OBJECT

  public:
    FontButton(QWidget *, QFont &);
    ~FontButton();
    void getFont(QFont &);
    void setFontButton();
    bool isChanged();

  public
    slots:
      void fontDialog();

    signals:
      void valueChanged();

  private:
    QFont font;
    bool changed;
};

#endif
