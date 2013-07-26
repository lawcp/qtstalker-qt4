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

#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include <qstring.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <q3dict.h>
#include <q3frame.h>
//Added by qt3to4:
#include <Q3GridLayout>
//#include <qpushbutton.h>

#include "ToolBarBtn.h"

class Toolbar : public Q3Frame
{
  public:
    enum Bias
    {
      Horizontal,
      Vertical
    };

    //Toolbar (QWidget *w, int h, int w, bool);
    Toolbar (QWidget *w, Bias);
    ~Toolbar ();
    void addButton (QString &name, QPixmap pix, QString &tt);
    ToolBarBtn * getButton (QString &name);
    void setButtonStatus (QString &name, bool d);
    void addSeparator ();

  private:
    ToolBarBtn *cancelButton;
    Q3Dict<ToolBarBtn> list;
//    int height;
//    int width;
    Q3GridLayout *grid;
    //bool pflag;
    Bias bias;
};

#endif
