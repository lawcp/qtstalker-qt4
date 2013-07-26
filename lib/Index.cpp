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

#include "Index.h"
#include "Config.h"
#include "DbPlugin.h"
#include <qdir.h>

Index::Index ()
{
}

Index::~Index ()
{
}

void Index::getHistory (BarData *barData, QDateTime &startDate, QString &il,
                           int barRange, BarData::BarLength barLength)
{
  QStringList l = QStringList::split(":", il, FALSE);
  if (! l.count())
    return;

  QDict<Bar> lookup;
  lookup.setAutoDelete(TRUE);
  int loop;
  int count = 0;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
//    emit signalProgMessage(loop, l.count());

    float weight = l[loop + 1].toFloat();
    if (weight == 0)
      weight = 1;

    loadIndexData(l[loop], lookup, startDate, weight, barRange, barLength);
    count++;
  }

  l.clear();
  QString s;
  QDictIterator<Bar> it(lookup);
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

//  emit signalProgMessage(-1, -1);
}

void Index::loadIndexData (QString &symbol, QDict<Bar> &lookup, QDateTime &startDate, float weight,
                           int barRange, BarData::BarLength barLength)
{
  DbPlugin db;
  if (db.openChart(symbol))
  {
    qDebug("DbPlugin::getIndexHistory: cannot open symbol chart");
    db.close();
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
  db.close();
}
