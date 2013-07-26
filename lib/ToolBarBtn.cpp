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


// This Class is intended to use at all tool bar buttons
// So it is more ease to keep a consistent look
// loh.tar

#include "ToolBarBtn.h"

ToolBarBtn::ToolBarBtn (QWidget *parent) : QPushButton (parent)
{
  // well, only one constant seems to be enough
  // but who knows what's up in the future
 /* btnMinWidth = 25;
  btnMaxWidth = 25;
  btnMinHeight = 25;
  btnMaxHeight = 25;
   */
  setMaximumWidth(btnMaxWidth);
  setMinimumWidth(btnMinWidth);
  setMaximumHeight(btnMaxHeight);
  setMinimumHeight(btnMinHeight);

}

ToolBarBtn::~ToolBarBtn ()
{
}
