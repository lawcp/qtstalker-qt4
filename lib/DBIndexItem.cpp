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

#include "DBIndexItem.h"
#include <string.h>

#define SYMBOL_LENGTH 15
#define TYPE_LENGTH 10
#define TITLE_LENGTH 90
#define PATH_LENGTH 100
#define QUOTEPLUGIN_LENGTH 15
#define FUTURESTYPE_LENGTH 5
#define FUTURESMONTH_LENGTH 2
#define EXCHANGE_LENGTH 10


DBIndexItem::DBIndexItem ()
{
  memset(&item, 0, sizeof(DBIndexItemItem));
}

DBIndexItem::~DBIndexItem ()
{
}

void DBIndexItem::getItem(DBIndexItemItem &d)
{
  d = item;
}

void DBIndexItem::setItem (DBIndexItemItem &d)
{
  item = d;
}

void DBIndexItem::setBarType (bool d)
{
  item.barType = d;
}

bool DBIndexItem::getBarType ()
{
  return item.barType;
}

void DBIndexItem::setSymbol (QString &d)
{
  strncpy(item.symbol, d, SYMBOL_LENGTH);
}

void DBIndexItem::getSymbol (QString &d)
{
  d = item.symbol;
}

void DBIndexItem::setType (QString &d)
{
  strncpy(item.type, d, TYPE_LENGTH);
}

void DBIndexItem::getType (QString &d)
{
  d = item.type;
}

void DBIndexItem::setTitle (QString &d)
{
  strncpy(item.title, d, TITLE_LENGTH);
}

void DBIndexItem::getTitle (QString &d)
{
  d = item.title;
}

void DBIndexItem::setPath (QString &d)
{
  strncpy(item.path, d, PATH_LENGTH);
}

void DBIndexItem::getPath (QString &d)
{
  d = item.path;
}

void DBIndexItem::setQuotePlugin (QString &d)
{
  strncpy(item.quotePlugin, d, QUOTEPLUGIN_LENGTH);
}

void DBIndexItem::getQuotePlugin (QString &d)
{
  d = item.quotePlugin;
}

void DBIndexItem::setFuturesType (QString &d)
{
  strncpy(item.futuresType, d, FUTURESTYPE_LENGTH);
}

void DBIndexItem::getFuturesType (QString &d)
{
  d = item.futuresType;
}

void DBIndexItem::setFuturesMonth (QString &d)
{
  strncpy(item.futuresMonth, d, FUTURESMONTH_LENGTH);
}

void DBIndexItem::getFuturesMonth (QString &d)
{
  d = item.futuresMonth;
}

void DBIndexItem::setExchange (QString &d)
{
  strncpy(item.exchange, d, EXCHANGE_LENGTH);
}

void DBIndexItem::getExchange (QString &d)
{
  d = item.exchange;
}
