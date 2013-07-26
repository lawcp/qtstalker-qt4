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

#ifndef SPREAD_HPP
#define SPREAD_HPP

#include <qstring.h>
#include <qdatetime.h>
#include "BarData.h"

class Spread
{
  public:

    Spread ();
    ~Spread ();
    void getHistory (BarData *, QDateTime &, QString &, QString &, int, BarData::BarLength);
    bool createNew (QString &path, QString &symbol);
    bool prefDialog (QString &fs, QString &ss);

  protected:
    QString helpFile;
};

#endif
