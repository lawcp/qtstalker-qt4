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

#ifndef DBINDEX_HPP
#define DBINDEX_HPP

#include <qstring.h>
#include <qstringlist.h>
#include "DBBase.h"
#include "DBIndexItem.h"
#include "Setting.h"

class DBIndex : public DBBase
{
  public:
    DBIndex();
    ~DBIndex();
    void setIndexItem(QString &, DBIndexItem &);
    void getIndexItem(QString &, DBIndexItem &);
    void deleteIndicator(QString &, QString &); // local indicators
    void addIndicator(QString &, QString &); // local indicators
    void getIndicators(QString &, QString &); // local indicators
    void deleteChart(QString &);
    void getChartObjects(QString &, QStringList &);
    void setChartObject(QString &, QString &, Setting &);
    void deleteChartObject(QString &, QString &);
    void deleteAllChartObjects(QString &k);
    void getNewChartObjectName(QString &k, QString &name);
    void setFundamentals(QString &, QString &);
    void getFundamentals(QString &, QString &);
};

#endif
