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

#include "DBBase.h"
#include <qfile.h>
#include <qfileinfo.h>
#include <q3textstream.h>


DBBase::DBBase ()
{
  db = 0;
}

DBBase::~DBBase ()
{
  close();
}

int DBBase::open (QString &d)
{
  int rc = db_create(&db, NULL, 0);
  if (rc)
  {
    qDebug("DBBase::open: %s", db_strerror(rc));
    return TRUE;
  }

  rc = db->open(db, NULL, (char *) d.latin1(), NULL, DB_BTREE, DB_CREATE, 0664);
  if (rc)
  {
    qDebug("DBBase::open: %s", db_strerror(rc));
    return TRUE;
  }

  symbol = d;

  QFileInfo fi(symbol);
  indexKey = fi.fileName();

  return FALSE;
}

void DBBase::close ()
{
  if (db)
  {
    db->close(db, 0);
    db = 0;
  }
}

void DBBase::getData (QString &k, QString &d)
{
  DBT key, data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  d.truncate(0);

  int ret = db->get(db, NULL, &key, &data, 0);
  if (ret == 0)
    d = (char *) data.data;
  else
  {
//    char *err = db_strerror(ret);
//    qDebug("DBBase::getData: %s", err);
  }
}

void DBBase::setData (QString &k, QString &d)
{
  DBT key, data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  data.data = (char *) d.latin1();
  data.size = d.length() + 1;

  int ret = db->put(db, NULL, &key, &data, 0);
  if (ret != 0)
  {
    char *err = db_strerror(ret);
    qDebug("DBBase::setData: %s", err);
  }
}

void DBBase::deleteData (QString &k)
{
  DBT key;
  memset(&key, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  int ret = db->del(db, NULL, &key, 0);
  if (ret != 0)
  {
//    char *err = db_strerror(ret);
//    qDebug("DBBase::deleteData: %s", err);
  }
}

void DBBase::dump (QString &d)
{
  QFile outFile(d);
  if (! outFile.open(QIODevice::WriteOnly))
    return;
  Q3TextStream outStream(&outFile);

  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  db->cursor(db, NULL, &cur, 0);

  while (! cur->c_get(cur, &key, &data, DB_NEXT))
    outStream << (char *) key.data << "=" << (char *) data.data << "\n";

  cur->c_close(cur);
  outFile.close();
}

void DBBase::getSymbol (QString &d)
{
  d = symbol;
}

void DBBase::flush ()
{
  if (db)
    db->sync(db, 0);
}

void DBBase::getIndexKey (QString &d)
{
  d = indexKey;
}
