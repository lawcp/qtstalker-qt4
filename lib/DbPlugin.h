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

#ifndef DBPLUGIN_HPP
#define DBPLUGIN_HPP

#include <qstring.h>
#include <qdatetime.h>
#include <q3dict.h>
#include "BarData.h"
#include "Bar.h"
#include "DBBase.h"
#include "DBIndex.h"

class DbPlugin : public DBBase
{
  public:

    enum DbType
    {
      Stock1,
      Futures1,
      Spread1,
      Index1,
      CC1
    };

    typedef struct
    {
      double open;
      double high;
      double low;
      double close;
      double volume;
      int oi;
    } DBBar;

    DbPlugin();
    virtual ~DbPlugin();
    void setBarLength(BarData::BarLength);
    void setBarRange(int);
    void getHelpFile(QString &);
    void getLastBar(Bar &);
    void getFirstBar(Bar &);
    void getAllBars(BarData *);
    void setBar(Bar &);
    void getBar(DBBar &, QString &, Bar &);
    DbPlugin::DbType getType(QString &);
    void getPrevBar(QDateTime &, Bar &);
    void getNextBar(QDateTime &, Bar &);
    void getSearchBar(QDateTime &, Bar &);
    void dump(QString &d, bool f);
    void setType(DbPlugin::DbType);
    bool open(QString &, DBIndex *);
    void getHistory(BarData *, QDateTime &);
    int dbPrefDialog();
    bool createNewStock();
    bool createNewFutures();
    bool createNewIndex(DBIndex *);
    bool createNewSpread(DBIndex *);
    bool createNewCC(DBIndex *);
    int stockPref();
    int futuresPref();
    int indexPref();
    int spreadPref();
    int ccPref();
    void getIndexHistory(BarData *, QDateTime &);
    void loadIndexData(QString &, Q3Dict<Bar> &, QDateTime &, float, int, BarData::BarLength);
    void getSpreadHistory(BarData *, QDateTime &);
    void getCCHistory(BarData *, QDateTime &);

  protected:
    int barRange;
    BarData::BarLength barLength;
    QString helpFile;
    DbPlugin::DbType type;
    DBIndex *chartIndex;
};

#endif
