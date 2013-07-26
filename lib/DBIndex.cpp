/*
 *  Qtstalker - Commodity/Stock Charting and Technical Analysis Platform
 *
 *  Copyright (C) 2001-2007  Stefan S. Stratigakos
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

#include "DBIndex.h"
#include "Config.h"
#include <qstringlist.h>
#include <qfileinfo.h>


DBIndex::DBIndex ()
{
}

DBIndex::~DBIndex ()
{
  close();
}

void DBIndex::setIndexItem (QString &k, DBIndexItem &d)
{
  DBT key, data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBIndexItemItem item;
  memset(&item, 0, sizeof(DBIndexItemItem));
  d.getItem(item);
  data.data = &item;
  data.size = sizeof(DBIndexItemItem);

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  db->put(db, NULL, &key, &data, 0);

  flush();
}

void DBIndex::getIndexItem (QString &k, DBIndexItem &d)
{
  DBT key, data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  DBIndexItemItem item;
  memset(&item, 0, sizeof(DBIndexItemItem));
  data.data = &item;
  data.ulen = sizeof(DBIndexItemItem);
  data.flags = DB_DBT_USERMEM;

  db->get(db, NULL, &key, &data, 0);

  d.setItem(item);
}

void DBIndex::deleteIndicator (QString &k, QString &d)
{
  // d = the path to the indicator

  QString s;
  Config config;
  config.getData(Config::LocalIndicatorsPath, s);
  DBBase li;
  if (li.open(s))
  {
    qDebug("DBIndex::deleteIndicator: could not open LI db");
    return;
  }

  li.getData(k, s);
  QStringList l = QStringList::split(",", s, FALSE);
  l.remove(d);
  if (l.count())
  {
    s = l.join(",");
    li.setData(k, s);
  }
  else
  {
    s = "";
    li.setData(k, s);
  }

  li.close();
}

void DBIndex::addIndicator (QString &k, QString &d)
{
  // d = the path to the indicator

  QString s;
  Config config;
  config.getData(Config::LocalIndicatorsPath, s);
  DBBase li;
  if (li.open(s))
  {
    qDebug("DBIndex::addIndicator: could not open LI db");
    return;
  }

  li.getData(k, s);
  QStringList l = QStringList::split(",", s, FALSE);
  int i = l.findIndex(d);
  if (i != -1)
    return;

  l.append(d);
  s = l.join(",");
  li.setData(k, s);
  li.close();
}

void DBIndex::getIndicators (QString &k, QString &d)
{
  QString s;
  Config config;
  config.getData(Config::LocalIndicatorsPath, s);
  DBBase li;
  if (li.open(s))
  {
    qDebug("DBIndex::addIndicator: could not open LI db");
    return;
  }

  li.getData(k, d);
  li.close();
}

void DBIndex::deleteChart (QString &d)
{
  // delete the index record
  QFileInfo fi(d);
  QString key = fi.fileName();
  deleteData(key);

  // delete the local indicator record
  Config config;
  QString s;
  config.getData(Config::LocalIndicatorsPath, s);
  DBBase tdb;
  tdb.open(s);
  tdb.deleteData(key);
  tdb.close();

  // delete the chart objects
  deleteAllChartObjects(key);

  // delete the fundamentals record
  config.getData(Config::FundamentalsPath, s);
  tdb.open(s);
  tdb.deleteData(key);
  tdb.close();
}

// ****************************************************************************
// ************************* CHART OBJECTS ************************************
// ****************************************************************************

void DBIndex::getChartObjects (QString &k, QStringList &d)
{
  d.clear();

  QString s;
  Config config;
  config.getData(Config::COPath, s);
  DBBase codb;
  if (codb.open(s))
  {
    qDebug("DBIndex::deleteChartObject: could not open co.db");
    return;
  }

  s = k + "_LIST";
  QString s2;
  codb.getData(s, s2);
  QStringList l = QStringList::split(",", s2, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    // key is symbol + number
    s = k + l[loop];
    codb.getData(s, s2);
    d.append(s2);
  }

  codb.close();
}

void DBIndex::setChartObject (QString &k, QString &d, Setting &set)
{
  QString s;
  Config config;
  config.getData(Config::COPath, s);
  DBBase codb;
  if (codb.open(s))
  {
    qDebug("DBIndex::setChartObject: could not open co.db");
    return;
  }

  s = k + "_LIST";
  QString s2;
  codb.getData(s, s2);
  QStringList l = QStringList::split(",", s2, FALSE);
  if (l.findIndex(d) == -1)
  {
    // enter new # list
    l.append(d);
    s2 = l.join(",");
    codb.setData(s, s2);
  }

  s = k + d;
  set.getString(s2);
  codb.setData(s, s2);
  codb.close();
}

void DBIndex::deleteChartObject (QString &k, QString &d)
{
  QString s;
  Config config;
  config.getData(Config::COPath, s);
  DBBase codb;
  if (codb.open(s))
  {
    qDebug("DBIndex::deleteChartObject: could not open co.db");
    return;
  }

  s = k + "_LIST";
  QString s2;
  codb.getData(s, s2);
  QStringList l = QStringList::split(",", s2, FALSE);
  l.remove(d);
  if (! l.count())
    s2 = "";
  else
    s2 = l.join(",");
  codb.setData(s, s2);

  s = k + d;
  codb.deleteData(s);
  codb.close();
}

void DBIndex::deleteAllChartObjects (QString &k)
{
  QString s;
  Config config;
  config.getData(Config::COPath, s);
  DBBase codb;
  if (codb.open(s))
  {
    qDebug("DBIndex::deleteAllChartObjects: could not open co.db");
    return;
  }

  // get the key list
  s = k + "_LIST";
  QString s2;
  codb.getData(s, s2);
  QStringList l = QStringList::split(",", s2, FALSE);

  // loop through the list and delete each co
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    s = k + l[loop];
    codb.deleteData(s);
  }

  // remove keys from the list
  s = k + "_LIST";
  s2 = "";
  codb.setData(s, s2);

  codb.close();
}

void DBIndex::getNewChartObjectName (QString &k, QString &name)
{
  QString s;
  Config config;
  config.getData(Config::COPath, s);
  DBBase codb;
  if (codb.open(s))
  {
    qDebug("DBIndex::getNewChartObjectName: could not open co.db");
    return;
  }

  // get the key list for the symbol
  s = k + "_LIST";
  QString s2;
  codb.getData(s, s2);
  QStringList l = QStringList::split(",", s2, FALSE);

  int loop = 0;
  while (1)
  {
    name = QString::number(loop);
    if (l.findIndex(name) != -1)
      loop++;
    else
      break;
  }

  codb.close();
}

// ****************************************************************************
// ************************* FUNDAMENTALS *************************************
// ****************************************************************************

void DBIndex::getFundamentals (QString &k, QString &d)
{
  QString s;
  Config config;
  config.getData(Config::FundamentalsPath, s);
  DBBase fdb;
  if (fdb.open(s))
  {
    qDebug("DBIndex::getFundamentals: could not open fund.db");
    return;
  }

  fdb.getData(k, d);
  fdb.close();
}

void DBIndex::setFundamentals (QString &k, QString &d)
{
  QString s;
  Config config;
  config.getData(Config::FundamentalsPath, s);
  DBBase fdb;
  if (fdb.open(s))
  {
    qDebug("DBIndex::setFundamentals: could not open fund.db");
    return;
  }

  fdb.setData(k, d);
  fdb.close();
}
