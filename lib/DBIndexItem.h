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

#ifndef DBIndexItem_HPP
#define DBIndexItem_HPP

#include <qstring.h>

typedef struct
{
  bool barType;
  char symbol[15];
  char type[10];
  char title[90];
  char exchange[10];
  char path[100];
  char quotePlugin[15];
  char futuresType[5];
  char futuresMonth[2];
} DBIndexItemItem;

class DBIndexItem
{
  public:
    DBIndexItem();
    ~DBIndexItem();
    void getItem(DBIndexItemItem &);
    void setItem(DBIndexItemItem &);
    void setBarType(bool);
    bool getBarType();
    void setSymbol(QString &);
    void getSymbol(QString &);
    void setType(QString &);
    void getType(QString &);
    void setTitle(QString &);
    void getTitle(QString &);
    void setPath(QString &);
    void getPath(QString &);
    void setQuotePlugin(QString &);
    void getQuotePlugin(QString &);
    void setFuturesType(QString &);
    void getFuturesType(QString &);
    void setFuturesMonth(QString &);
    void getFuturesMonth(QString &);
    void setExchange(QString &);
    void getExchange(QString &);

  protected:
    DBIndexItemItem item;
};

#endif
