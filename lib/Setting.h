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

#ifndef SETTING_HPP
#define SETTING_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <q3dict.h>

class Setting
{
  public:
    Setting();
    ~Setting();
    void getData(QString &, QString &);
    double getDouble(QString &);
    int getInt(QString &);
    void setData(QString &, QString &);
    void getKeyList(QStringList &);
    void remove(QString &);
    void getString(QString &);
    void parse(QString &);
    void clear();
    int count();
    void copy(Setting *);

  protected:
    Q3Dict<QString> dict;
};

#endif
