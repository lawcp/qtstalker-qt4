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

#include "DbPlugin.h"
#include "StocksDialog.h"
#include "FuturesDialog.h"
#include "IndexDialog.h"
#include "Config.h"
#include "PrefDialog.h"
#include "FuturesData.h"
#include "Setting.h"
#include "DBIndexItem.h"
#include <qfileinfo.h>
#include <qfile.h>
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <q3valuelist.h>
#include <qstringlist.h>
#include <Q3TextStream>


DbPlugin::DbPlugin ()
{
  barLength = BarData::DailyBar;
  barRange = 275;
  type = Stock1;
}

DbPlugin::~DbPlugin ()
{
  close();
}

bool DbPlugin::open (QString &d, DBIndex *i)
{
  if (DBBase::open(d))
    return TRUE;

  chartIndex = i;

  QString s;
  DBIndexItem item;
  chartIndex->getIndexItem(indexKey, item);
  item.getType(s);
  type = getType(s);

  item.getPath(s);
  if (! s.length())
  {
    item.setPath(d);
    chartIndex->setIndexItem(indexKey, item);
  }

  return FALSE;
}

void DbPlugin::setBarLength (BarData::BarLength d)
{
  barLength = d;
}

void DbPlugin::setBarRange (int d)
{
  barRange = d;
}

void DbPlugin::getHelpFile (QString &d)
{
  d = helpFile;
}

void DbPlugin::setType (DbPlugin::DbType d)
{
  type = d;
}

void DbPlugin::dump (QString &d, bool f)
{
  QFile outFile(d);
  if (! outFile.open(QIODevice::WriteOnly))
    return;
  Q3TextStream outStream(&outFile);

  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);

  QFileInfo fi(symbol);

  while (! cur->c_get(cur, &key, &data, DB_NEXT))
  {
    if (f)
    {
      outStream << fi.fileName() << "," << (char *) key.data << "," << QString::number(dbbar.open) << ","
      << QString::number(dbbar.high) << "," << QString::number(dbbar.low) << ","
      << QString::number(dbbar.close) << "," << QString::number(dbbar.volume, 'f', 0) << ","
      << QString::number(dbbar.oi) << "\n";
    }
    else
    {
      outStream << (char *) key.data << "=" << QString::number(dbbar.open) << ","
      << QString::number(dbbar.high) << "," << QString::number(dbbar.low) << ","
      << QString::number(dbbar.close) << "," << QString::number(dbbar.volume, 'f', 0) << ","
      << QString::number(dbbar.oi) << "\n";
    }
  }
  cur->c_close(cur);

  outFile.close();
}

void DbPlugin::getFirstBar (Bar &bar)
{
  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);
  cur->c_get(cur, &key, &data, DB_NEXT);
  QString k = (char *) key.data;
  getBar(dbbar, k, bar);
  cur->c_close(cur);
}

void DbPlugin::getLastBar (Bar &bar)
{
  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);
  cur->c_get(cur, &key, &data, DB_PREV);
  QString k = (char *) key.data;
  getBar(dbbar, k, bar);
  cur->c_close(cur);
}

void DbPlugin::getPrevBar (QDateTime &startDate, Bar &bar)
{
  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);

  QString s = startDate.toString("yyyyMMddhhmmss");
  key.data = (char *) s.latin1();
  key.size = s.length() + 1;
  cur->c_get(cur, &key, &data, DB_SET_RANGE);
  int ret = cur->c_get(cur, &key, &data, DB_PREV);
  if (ret)
  {
//    char *err = db_strerror(ret);
//    qDebug("%s %s", s.latin1(), err);
    cur->c_close(cur);
    return;
  }
  s = (char *) key.data;
  getBar(dbbar, s, bar);
  cur->c_close(cur);
}

void DbPlugin::getNextBar (QDateTime &startDate, Bar &bar)
{
  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);

  QString s = startDate.toString("yyyyMMddhhmmss");
  key.data = (char *) s.latin1();
  key.size = s.length() + 1;
  cur->c_get(cur, &key, &data, DB_SET_RANGE);
  int ret = cur->c_get(cur, &key, &data, DB_NEXT);
  if (ret)
  {
//    char *err = db_strerror(ret);
//    qDebug("%s %s", s.latin1(), err);
    cur->c_close(cur);
    return;
  }
  s = (char *) key.data;
  getBar(dbbar, s, bar);
  cur->c_close(cur);
}

void DbPlugin::getSearchBar (QDateTime &startDate, Bar &bar)
{
  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);

  QString s = startDate.toString("yyyyMMddhhmmss");
  key.data = (char *) s.latin1();
  key.size = s.length() + 1;
  cur->c_get(cur, &key, &data, DB_SET_RANGE);
  s = (char *) key.data;
  getBar(dbbar, s, bar);
  cur->c_close(cur);
}

void DbPlugin::getHistory (BarData *barData, QDateTime &startDate)
{
  switch (type)
  {
    case Index1:
      getIndexHistory(barData, startDate);
      barData->createDateList();
      return;
      break;
    case Spread1:
      getSpreadHistory(barData, startDate);
      barData->createDateList();
      return;
      break;
    case CC1:
      getCCHistory(barData, startDate);
      barData->createDateList();
      return;
      break;
    default:
      break;
  }

  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);

  QString s = startDate.toString("yyyyMMddhhmmss");
  key.data = (char *) s.latin1();
  key.size = s.length() + 1;
  cur->c_get(cur, &key, &data, DB_SET_RANGE);

  while (! cur->c_get(cur, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
    Bar bar;
    s = (char *) key.data;
    getBar(dbbar, s, bar);
    bar.setTickFlag(barData->getBarType());
    barData->prepend(bar);
  }

  cur->c_close(cur);
  barData->createDateList();
}

void DbPlugin::getAllBars (BarData *bars)
{
  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.ulen = sizeof(DBBar);
  data.flags = DB_DBT_USERMEM;

  db->cursor(db, NULL, &cur, 0);

  while (! cur->c_get(cur, &key, &data, DB_PREV))
  {
    Bar bar;
    QString s = (char *) key.data;
    getBar(dbbar, s, bar);
    bar.setTickFlag(bars->getBarType());
    bars->prependRaw(bar);
  }

  cur->c_close(cur);
}

DbPlugin::DbType DbPlugin::getType (QString &d)
{
  DbPlugin::DbType t = Stock1;

  while (1)
  {
    if (! d.compare("Stock"))
    {
      t = Stock1;
      break;
    }

    if (! d.compare("Futures"))
    {
      t = Futures1;
      break;
    }

    if (! d.compare("Spread"))
    {
      t = Spread1;
      break;
    }

    if (! d.compare("Index"))
    {
      t = Index1;
      break;
    }

    if (! d.compare("CC"))
    {
      t = CC1;
      break;
    }

    break;
  }

  return t;
}

void DbPlugin::getBar (DBBar &d, QString &k, Bar &bar)
{
  if (bar.setDate(k))
    return;
  bar.setOpen(d.open);
  bar.setHigh(d.high);
  bar.setLow(d.low);
  bar.setClose(d.close);
  bar.setVolume(d.volume);
  bar.setOI(d.oi);
}

void DbPlugin::setBar (Bar &bar)
{
  DBT key, data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  DBBar dbbar;
  memset(&dbbar, 0, sizeof(DBBar));
  data.data = &dbbar;
  data.size = sizeof(DBBar);

  QString s;
  bar.getDateTimeString(FALSE, s);
  key.data = (char *) s.latin1();
  key.size = s.length() + 1;
  dbbar.open = bar.getOpen();
  dbbar.high = bar.getHigh();
  dbbar.low = bar.getLow();
  dbbar.close = bar.getClose();
  dbbar.volume = bar.getVolume();
  dbbar.oi = (int) bar.getOI();

  db->put(db, NULL, &key, &data, 0);
}

bool DbPlugin::createNewStock ()
{
  QString s;
  DBIndexItem item;
  chartIndex->getIndexItem(indexKey, item);
  item.getSymbol(s);
  if (s.length())
  {
    qDebug("DbPlugin::createNewStock: duplicate symbol %s", s.latin1());
    return TRUE;
  }

  type = Stock1;
  s = "Stock";
  item.setType(s);
  item.setTitle(indexKey);
  item.setSymbol(indexKey);
  chartIndex->setIndexItem(indexKey, item);
  return FALSE;
}

bool DbPlugin::createNewFutures ()
{
  QString s;
  DBIndexItem item;
  chartIndex->getIndexItem(indexKey, item);
  item.getSymbol(s);
  if (s.length())
  {
    qDebug("DbPlugin::createNewStock: duplicate symbol %s", s.latin1());
    return TRUE;
  }

  type = Futures1;
  FuturesData fd;
  if (fd.setSymbolPath(symbol))
    return TRUE;

  s = "Futures";
  item.setType(s);

  fd.getName(s);
  item.setTitle(s);

  item.setSymbol(indexKey);

  fd.getSymbol(s);
  item.setFuturesType(s);

  s = symbol.right(1);
  item.setFuturesMonth(s);

  chartIndex->setIndexItem(indexKey, item);

  return FALSE;
}

bool DbPlugin::createNewIndex (DBIndex *i)
{
  bool ok = FALSE;
  QString sym = QInputDialog::getText(QObject::tr("New Index"),
                                      QObject::tr("Enter symbol name for the new Index"),
                                      QLineEdit::Normal,
                                      QString::null,
                                      &ok,
                                      0);
  if (! sym.length() || ok == FALSE)
    return TRUE;

  QDir dir;
  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/Index");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
                               QObject::tr("Could not create ~/.qtstalker/data/Index directory."));
      return TRUE;
    }
  }

  s.append("/" + sym);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
                             QObject::tr("This Index already exists."));
    return TRUE;
  }

  DBIndexItem item;
  QString ts;
  chartIndex->getIndexItem(sym, item);
  item.getSymbol(ts);
  if (ts.length())
  {
    qDebug("DbPlugin::createNewStock: duplicate symbol %s", ts.latin1());
    return TRUE;
  }

  if (open(s, i))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
                             QObject::tr("Disk error, cannot create chart"));
    return TRUE;
  }

  type = Index1;
  item.setSymbol(sym);
  s = "Index";
  item.setType(s);
  item.setTitle(sym);
  chartIndex->setIndexItem(indexKey, item);

  indexPref();
  return FALSE;
}

bool DbPlugin::createNewSpread (DBIndex *i)
{
  bool ok = FALSE;
  QString sn = QInputDialog::getText(QObject::tr("New Spread"),
                                     QObject::tr("Enter symbol name for the new Spread"),
                                     QLineEdit::Normal,
                                     QString::null,
                                     &ok,
                                     0);
  if (! sn.length() || ok == FALSE)
    return TRUE;

  QDir dir;
  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/Spread");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0, QObject::tr("Qtstalker: Error"), QObject::tr("Could not create Spread directory."));
      return TRUE;
    }
  }

  s.append("/" + sn);
  if (dir.exists(s))
  {
    QMessageBox::information(0, QObject::tr("Qtstalker: Error"), QObject::tr("This Spread already exists."));
    return TRUE;
  }

  DBIndexItem item;
  QString ts;
  chartIndex->getIndexItem(sn, item);
  item.getSymbol(ts);
  if (ts.length())
  {
    qDebug("DbPlugin::createNewStock: duplicate symbol %s", sn.latin1());
    return TRUE;
  }


  if (open(s, i))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
                             QObject::tr("Disk error, cannot create chart"));
    return TRUE;
  }

  type = Spread1;
  item.setSymbol(indexKey);
  s = "Spread";
  item.setType(s);
  item.setTitle(indexKey);
  chartIndex->setIndexItem(indexKey, item);

  spreadPref();
  return FALSE;
}

bool DbPlugin::createNewCC (DBIndex *i)
{
  FuturesData fd;
  QStringList l;
  fd.getSymbolList(l);

  QString pl = QObject::tr("Parms");
  QString fsl = QObject::tr("Futures Symbol");
  QString gl = QObject::tr("Gapless");

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("New CC"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  dialog->addComboItem(fsl, pl, l, 0);
  dialog->addCheckItem(gl, pl, TRUE);

  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return TRUE;
  }

  QString sym;
  dialog->getCombo(fsl, sym);
  bool f = dialog->getCheck(gl);

  delete dialog;

  QDir dir;
  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/CC");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
                               QObject::tr("Could not create ~/.qtstalker/data/CC directory."));
      return TRUE;
    }
  }

  DBIndexItem item;
  QString ts;
  chartIndex->getIndexItem(sym, item);
  item.getSymbol(ts);
  if (ts.length())
  {
    qDebug("DbPlugin::createNewStock: duplicate symbol %s", ts.latin1());
    return TRUE;
  }


  s.append("/" + sym);
  if (open(s, i))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
                             QObject::tr("Disk error, cannot create chart"));
    return TRUE;
  }

  type = CC1;
  item.setSymbol(sym);
  s = "CC";
  item.setType(s);
  s = sym + " - " + QObject::tr("Continuous Adjusted");
  item.setTitle(s);
  chartIndex->setIndexItem(indexKey, item);

  s = QString::number(f);
  sym = "Adjustment";
  setData(sym, s);

  return FALSE;
}

int DbPlugin::dbPrefDialog ()
{
  int rc = 0;

  switch (type)
  {
    case Futures1:
      rc = futuresPref();
      break;
    case Index1:
      rc = indexPref();
      break;
    case Spread1:
      rc = spreadPref();
      break;
    case CC1:
      rc = ccPref();
      break;
    default:
      rc = stockPref();
      break;
  }

  return rc;
}

int DbPlugin::stockPref ()
{
  int rc = 0;
  // FIXME: "helpFile" should not be set here,
  // I don't know right now where better placed
  helpFile = "stocksplugin.html";
  StocksDialog *dialog = new StocksDialog(helpFile, this, chartIndex);
  dialog->exec();
  rc = dialog->getReloadFlag();
  delete dialog;
  chartIndex->flush();
  return rc;
}

int DbPlugin::futuresPref ()
{
  int rc = 0;
  FuturesDialog *dialog = new FuturesDialog(helpFile, this, chartIndex);
  dialog->exec();
  rc = dialog->getReloadFlag();
  delete dialog;
  chartIndex->flush();
  return rc;
}

int DbPlugin::indexPref ()
{
  int rc = 0;
  QString nam, il;
  DBIndexItem item;
  chartIndex->getIndexItem(indexKey, item);
  item.getTitle(nam);

  QString s = "List";
  getData(s, il);

  IndexDialog *dialog = new IndexDialog(nam, il);
  int trc = dialog->exec();
  if (trc == QDialog::Accepted)
  {
    dialog->getName(nam);
    item.setTitle(nam);
    dialog->getList(il);
    setData(s, il);
    chartIndex->setIndexItem(indexKey, item);
    rc = TRUE;
  }
  delete dialog;
  return rc;
}

int DbPlugin::spreadPref ()
{
  int rc = 0;
  QString pl = QObject::tr("Parms");
  QString fsl = QObject::tr("First Symbol");
  QString ssl = QObject::tr("Second Symbol");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Edit Spread"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  Config config;
  QString s, s3;
  config.getData(Config::DataPath, s);

  QString s2 = "FirstSymbol";
  getData(s2, s3);
  dialog->addSymbolItem(fsl, pl, s, s3);

  s2 = "SecondSymbol";
  getData(s2, s3);
  dialog->addSymbolItem(ssl, pl, s, s3);

  int trc = dialog->exec();
  if (trc == QDialog::Accepted)
  {
    dialog->getSymbol(fsl, s);
    s2 = "FirstSymbol";
    setData(s2, s);

    dialog->getSymbol(ssl, s);
    s2 = "SecondSymbol";
    setData(s2, s);
    rc = TRUE;
  }

  delete dialog;
  return rc;
}

int DbPlugin::ccPref ()
{
  int rc = 0;
  QString pl = QObject::tr("Parms");
  QString gl = QObject::tr("Gapless");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Edit CC"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s = "Adjustment";
  QString s2;
  getData(s, s2);
  dialog->addCheckItem(gl, pl, s2.toInt());

  int trc = dialog->exec();
  if (trc == QDialog::Accepted)
  {
    s2 = QString::number(dialog->getCheck(gl));
    setData(s, s2);
    rc = TRUE;
  }

  delete dialog;
  return rc;
}

void DbPlugin::getIndexHistory (BarData *barData, QDateTime &startDate)
{
  QString s = "List";
  QString s2;
  getData(s, s2);
  QStringList l = QStringList::split(":", s2, FALSE);
  if (! l.count())
    return;

  Q3Dict<Bar> lookup;
  lookup.setAutoDelete(TRUE);
  int loop;
  int count = 0;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    float weight = l[loop + 1].toFloat();
    if (weight == 0)
      weight = 1;

    loadIndexData(l[loop], lookup, startDate, weight, barRange, barLength);
    count++;
  }

  l.clear();
  Q3DictIterator<Bar> it(lookup);
  for (; it.current(); ++it)
  {
    Bar *r = it.current();
    if (r->getOI() == count)
    {
      r->setOpen(r->getOpen() / count);
      r->setHigh(r->getHigh() / count);
      r->setLow(r->getLow() / count);
      r->setClose(r->getClose() / count);

      if (r->getOpen() > r->getHigh())
        r->setHigh(r->getOpen());
      if (r->getOpen() < r->getLow())
        r->setLow(r->getOpen());

      if (r->getClose() > r->getHigh())
        r->setHigh(r->getClose());
      if (r->getClose() < r->getLow())
        r->setLow(r->getClose());

      r->getDateTimeString(FALSE, s);
      l.append(s);
    }
    else
      lookup.remove(it.currentKey());
  }

  l.sort();
  for (loop = l.count() - 1; loop > -1; loop--)
  {
    Bar *r = lookup.find(l[loop]);
    if (r)
    {
      QDateTime dt;
      r->getDate(dt);
      Bar tr;
      tr.setDate(dt);
      tr.setOpen(r->getOpen());
      tr.setHigh(r->getHigh());
      tr.setLow(r->getLow());
      tr.setClose(r->getClose());
      barData->prepend(tr);
    }
  }
}

void DbPlugin::loadIndexData (QString &symbol, Q3Dict<Bar> &lookup, QDateTime &startDate, float weight,
                           int barRange, BarData::BarLength barLength)
{
  QFileInfo fi(symbol);
  QString fn = fi.fileName();

  DbPlugin db;
  if (db.open(symbol, chartIndex))
  {
    qDebug("Index::getIndexHistory: cannot open symbol chart");
    return;
  }

  BarData *bar = new BarData(symbol);
  bar->setBarLength(barLength);
  db.setBarRange(barRange);
  db.getHistory(bar, startDate);
  db.close();

  int loop;
  for (loop = 0; loop < (int) bar->count(); loop++)
  {
    QDateTime dt;
    bar->getDate(loop, dt);
    QString s = dt.toString("yyyyMMddhhmmss");
    Bar *r = lookup.find(s);
    if (! r)
    {
      r = new Bar;
      r->setDate(dt);
      r->setOpen(bar->getOpen(loop) * weight);
      r->setHigh(bar->getHigh(loop) * weight);
      r->setLow(bar->getLow(loop) * weight);
      r->setClose(bar->getClose(loop) * weight);
      r->setOI(1);
      r->getDateTimeString(FALSE, s);
      lookup.insert(s, r);
    }
    else
    {
      r->setOpen(r->getOpen() + (bar->getOpen(loop) * weight));
      r->setHigh(r->getHigh() + (bar->getHigh(loop) * weight));
      r->setLow(r->getLow() + (bar->getLow(loop) * weight));
      r->setClose(r->getClose() + (bar->getClose(loop) * weight));
      r->setOI((int) r->getOI() + 1);
    }
  }

  delete bar;
}

void DbPlugin::getSpreadHistory (BarData *barData, QDateTime &startDate)
{
  QString s = "FirstSymbol";
  QString fs;
  getData(s, fs);

  QString ss;
  s = "SecondSymbol";
  getData(s, ss);

  // get the first symbol bars
  QFileInfo fi(fs);
  QString fn = fi.fileName();

  DbPlugin db;
  if (db.open(fs, chartIndex))
  {
    qDebug("Spread::getSpreadHistory: cannot open first symbol chart");
    return;
  }

  BarData *bar = new BarData(fs);
  bar->setBarLength(barLength);
  db.setBarRange(barRange);
  db.getHistory(bar, startDate);
  db.close();

  // get the second symbol bars
  QFileInfo fi2(ss);
  fn = fi2.fileName();

  if (db.open(ss, chartIndex))
  {
    qDebug("Spread::getSpreadHistory: cannot open second symbol chart");
    delete bar;
    return;
  }

  BarData *bar2 = new BarData(ss);
  bar2->setBarLength(barLength);
  db.setBarRange(barRange);
  db.getHistory(bar2, startDate);
  db.close();

  // create lookup dict for first symbol bars
  Q3Dict<Bar> lookup;
  lookup.setAutoDelete(TRUE);
  int loop;
  for (loop = 0; loop < bar->count(); loop++)
  {
    Bar *r = new Bar;
    QDateTime dt;
    bar->getDate(loop, dt);
    r->setDate(dt);
    r->setClose(bar->getClose(loop));
    r->getDateTimeString(FALSE, s);
    lookup.insert(s, r);
  }

  // match all second symbol bars
  for (loop = bar2->count() - 1; loop > -1; loop--)
  {
    Bar r;
    QDateTime dt;
    bar2->getDate(loop, dt);
    s = dt.toString("yyyyMMddhhmmss");
    Bar *tr = lookup.find(s);
    if (tr)
    {
      double t = tr->getClose() - bar2->getClose(loop);
      r.setDate(dt);
      r.setOpen(t);
      r.setHigh(t);
      r.setLow(t);
      r.setClose(t);
      barData->prepend(r);
    }
  }

  delete bar;
  delete bar2;
}

void DbPlugin::getCCHistory (BarData *barData, QDateTime &startDate)
{
  FuturesData fd;
  if (fd.setSymbol(indexKey))
  {
    qDebug("CC::getCCHistory: invalid futures symbol");
    return;
  }

  Config config;
  QString s;
  QString baseDir;
  config.getData(Config::DataPath, baseDir);
  baseDir.append("/Futures/");
  fd.getExchange(s);
  baseDir.append(s + "/");
  fd.getSymbol(s);
  baseDir.append(s);
  QDir dir(baseDir);
  if (! dir.exists(baseDir))
    return;
  QStringList dirList = dir.entryList();

  QString lastChart;
  fd.getCurrentContract(startDate, lastChart);
  QString ey = lastChart.right(5);
  ey.truncate(4);

  Q3ValueList<Bar> indexList;
  int indexCount = -1;

  int dirLoop = dirList.findIndex(lastChart);
  if (dirLoop == -1)
    dirLoop = dirList.count() - 1;
  lastChart = dirList[dirLoop];

  s = "Adjustment";
  QString s2;
  getData(s, s2);
  bool adjustFlag = s2.toInt();

  while (dirLoop > 1)
  {
    if (indexCount >= barRange)
      break;

    s = baseDir + "/" + dirList[dirLoop];

    DbPlugin tdb;
    if (tdb.open(s, chartIndex))
    {
      tdb.close();
      dirLoop--;
      lastChart = dirList[dirLoop];
      continue;
    }

    BarData *recordList = new BarData(s);
    tdb.setBarRange(barRange);
    tdb.setBarLength(barLength);
    tdb.getHistory(recordList, startDate);
    tdb.close();

    int loop;
    QDateTime dt = startDate;
    int lastBar = -1;
    bool dataFlag = FALSE;
    for (loop = recordList->count() - 1; loop > -1; loop--)
    {
      if (indexCount >= barRange)
        break;

      recordList->getDate(loop, dt);
      fd.getCurrentContract(dt, s);
      if (! s.compare(lastChart))
      {
        Bar bar;
        recordList->getBar(loop, bar);
        indexList.prepend(bar);
        indexCount++;
        startDate = dt;
        lastBar = loop;
        dataFlag = TRUE;
      }
    }

    if (dataFlag)
    {
      if (adjustFlag)
      {
        Bar bar;
        double t = 0;
        if (lastBar - 1 > -1)
          t = recordList->getClose(lastBar) - recordList->getClose(lastBar - 1);
        bar.setClose(t);
        bar.setEmptyFlag(TRUE);
        indexList.prepend(bar);
      }
    }

    delete recordList;

    dirLoop--;
    lastChart = dirList[dirLoop];
  }

  if (! adjustFlag)
  {
    int loop;
    for (loop = 0; loop < (int) indexList.count(); loop++)
    {
      Bar bar = indexList[loop];
      barData->appendRaw(bar);
    }

    return;
  }

  // adjust the data
  double adjust = 0;
  double t = 0;
  bool flag = FALSE;
  Bar prevBar;
  int loop;
  for (loop = 1; loop < (int) indexList.count(); loop++)
  {
    Bar bar = indexList[loop];

    if (bar.getEmptyFlag())
    {
      t = bar.getClose();
      flag = TRUE;
      continue;
    }

    if (flag)
    {
      adjust = prevBar.getClose() - bar.getClose();
      bar.setOpen(bar.getOpen() + t);
      bar.setHigh(bar.getHigh() + t);
      bar.setLow(bar.getLow() + t);
      bar.setClose(bar.getClose() + t);
      flag = FALSE;
      t = 0;
    }

    bar.setOpen(bar.getOpen() + adjust);
    bar.setHigh(bar.getHigh() + adjust);
    bar.setLow(bar.getLow() + adjust);
    bar.setClose(bar.getClose() + adjust);
    barData->appendRaw(bar);
    prevBar = bar;
  }
}
