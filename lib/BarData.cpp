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

#include "BarData.h"
#include <qobject.h>

BarData::BarData (QString &sym)
{
  symbol = sym;
  high = -99999999;
  low = 99999999;
  dateList.setAutoDelete(TRUE);
  barType = Daily;
  barLength = DailyBar;
}

BarData::~BarData ()
{
}

void BarData::getInputFields (QStringList &l)
{
  l.clear();
  l.append(QObject::tr("Open"));
  l.append(QObject::tr("High"));
  l.append(QObject::tr("Low"));
  l.append(QObject::tr("Close"));
  l.append(QObject::tr("Volume"));
  l.append(QObject::tr("OpenInterest"));
  l.append(QObject::tr("Day"));
  l.append(QObject::tr("Week"));
  l.append(QObject::tr("Month"));
  l.append(QObject::tr("DayOfWeek"));
}

PlotLine * BarData::getInput (BarData::InputType field)
{
  PlotLine *in = new PlotLine();
  int loop;
  QDateTime dt;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    switch(field)
    {
      case Open:
        in->append(getOpen(loop));
	break;
      case High:
        in->append(getHigh(loop));
	break;
      case Low:
        in->append(getLow(loop));
	break;
      case Volume:
        in->append(getVolume(loop));
	break;
      case OpenInterest:
        in->append(getOI(loop));
	break;
      case Day:
        getDate(loop, dt);
        in->append(dt.date().day());
	break;
      case Week:
        getDate(loop, dt);
        in->append(dt.date().weekNumber());
	break;
      case Month:
        getDate(loop, dt);
        in->append(dt.date().month());
	break;
      case DayOfWeek:
        getDate(loop, dt);
        in->append(dt.date().dayOfWeek());
	break;
      default:
        in->append(getClose(loop));
        break;
    }
  }

  return in;
}

int BarData::count ()
{
  return (int) barList.count();
}

void BarData::prepend (Bar &bar)
{
  QDateTime dt;
  bar.getDate(dt);

  if (! currentBar.getEmptyFlag())
  {
    if (dt >= startDate && dt < endDate)
    {
      currentBar.setOpen(bar.getOpen());
      if (bar.getHigh() > currentBar.getHigh())
        currentBar.setHigh(bar.getHigh());
      if (bar.getLow() < currentBar.getLow())
        currentBar.setLow(bar.getLow());
//    if (barType == Tick)
        currentBar.setVolume(currentBar.getVolume() + bar.getVolume());
      if (bar.getOI() > currentBar.getOI())
        currentBar.setOI((int) bar.getOI());
    }
    else
    {
      if (currentBar.getHigh() > high)
        high = currentBar.getHigh();
      if (currentBar.getLow() < low)
        low = currentBar.getLow();

      barList.prepend(currentBar);
      currentBar.clear();

      setStartEndDates(dt);

      currentBar = bar;

      if (barLength > Minute60)
        currentBar.setDate(dt); // test
      else
        currentBar.setDate(endDate); // test
    }
  }
  else
  {
    currentBar.clear();
    setStartEndDates(dt);
    currentBar = bar;
    if (barLength > Minute60)
      currentBar.setDate(dt); // test
    else
      currentBar.setDate(endDate); // test
  }
}

void BarData::prependRaw (Bar &bar)
{
  barList.prepend(bar);
}

void BarData::appendRaw (Bar &bar)
{
  barList.append(bar);
}

void BarData::createDateList ()
{
  if (! currentBar.getEmptyFlag())
  {
    if (currentBar.getHigh() > high)
      high = currentBar.getHigh();
    if (currentBar.getLow() < low)
      low = currentBar.getLow();
    barList.prepend(currentBar);
    currentBar.clear();
  }

  dateList.clear();

  int loop;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    Bar bar = barList[loop];

    X *x = new X;
    x->x = loop;

    QString s;
    bar.getDateTimeString(FALSE, s);
    dateList.replace(s, x);
  }
}

void BarData::getDate (int i, QDateTime &dt)
{
  barList[i].getDate(dt);
}

int BarData::getX (QDateTime &date)
{
  X *x = 0;
  QString s = date.toString("yyyyMMddhhmmss");
  x = dateList[s];

  if (x)
    return x->x;
  else
    return -1;

/*
  if (barCompression == Weekly)
  {
    QDateTime dt = date;
    dt = dt.addDays(- (dt.date().dayOfWeek() - 1));

    int loop = 0;
    for (loop = 0; loop < 6; loop++)
    {
      X *x = dateList[dt.toString("yyyyMMdd")];
      if (x)
       return x->x;
      else
        dt = dt.addDays(1);
    }

    return -1;
  }

  if (barCompression == Monthly)
  {
    QDateTime dt = date;
    dt.date().setYMD(date.date().year(), 1, 1);

    int loop = 0;
    for (loop = 0; loop < dt.date().daysInMonth(); loop++)
    {
      X *x = dateList[dt.toString("yyyyMMdd")];
      if (x)
       return x->x;
      else
        dt = dt.addDays(1);
    }

    return -1;
  }

  return -1;
*/
}

double BarData::getOpen (int i)
{
  return barList[i].getOpen();
}

double BarData::getHigh (int i)
{
  return barList[i].getHigh();
}

double BarData::getLow (int i)
{
  return barList[i].getLow();
}

double BarData::getClose (int i)
{
  return barList[i].getClose();
}

double BarData::getVolume (int i)
{
  return barList[i].getVolume();
}

double BarData::getOI (int i)
{
  return barList[i].getOI();
}

double BarData::getMax ()
{
  return high;
}

double BarData::getMin ()
{
  return low;
}

void BarData::setBarType (BarData::BarType d)
{
  barType = d;
}

BarData::BarType BarData::getBarType ()
{
  return barType;
}

BarData::InputType BarData::getInputType (QString &d)
{
  InputType t = Close;

  while (1)
  {
    if (! d.compare(QObject::tr("Open")))
    {
      t = Open;
      break;
    }

    if (! d.compare(QObject::tr("High")))
    {
      t = High;
      break;
    }

    if (! d.compare(QObject::tr("Low")))
    {
      t = Low;
      break;
    }

    if (! d.compare(QObject::tr("Close")))
    {
      t = Close;
      break;
    }

    if (! d.compare(QObject::tr("Volume")))
    {
      t = Volume;
      break;
    }

    if (! d.compare(QObject::tr("OpenInterest")))
    {
      t = OpenInterest;
      break;
    }

    if (! d.compare(QObject::tr("Day")))
    {
      t = Day;
      break;
    }

    if (! d.compare(QObject::tr("Week")))
    {
      t = Week;
      break;
    }

    if (! d.compare(QObject::tr("Month")))
    {
      t = Month;
      break;
    }

    if (! d.compare(QObject::tr("DayofWeek")))
    {
      t = DayOfWeek;
      break;
    }

    break;
  }

  return t;
}

void BarData::getBarLengthList (QStringList &l)
{
  l.clear();
  l.append(QObject::tr("1 Minute"));
  l.append(QObject::tr("5 Minute"));
  l.append(QObject::tr("10 Minute"));
  l.append(QObject::tr("15 Minute"));
  l.append(QObject::tr("30 Minute"));
  l.append(QObject::tr("1 Hour"));
  l.append(QObject::tr("Daily"));
  l.append(QObject::tr("Weekly"));
  l.append(QObject::tr("Monthly"));
}

void BarData::getBar (int d, Bar &bar)
{
  bar = barList[d];
}

void BarData::setBar (int d, Bar &bar)
{
  barList[d] = bar;
}

void BarData::setMinMax ()
{
  int loop;
  for (loop = 0; loop < (int) barList.count(); loop++)
  {
    Bar bar = barList[loop];

    if (bar.getHigh() > high)
      high = bar.getHigh();

    if (bar.getLow() < low)
      low = bar.getLow();
  }
}

void BarData::clear ()
{
  high = -99999999;
  low = 99999999;
  dateList.clear();
  barList.clear();
  currentBar.clear();
}

void BarData::setBarLength (BarData::BarLength d)
{
  barLength = d;
}

BarData::BarLength BarData::getBarLength ()
{
  return barLength;
}

void BarData::setStartEndDates (QDateTime &d)
{
  QString s, s2;
  int tint = 0;
  QDateTime dt = d;

  switch (barLength)
  {
    case Minute1:
      dt.setTime(QTime(dt.time().hour(), dt.time().minute(), 0, 0));
      startDate = dt;
      dt = dt.addSecs(60);
      endDate = dt;
      break;
    case Minute5:
      tint = dt.time().minute() / 5;
      dt.setTime(QTime(dt.time().hour(), tint * 5, 0, 0));
      startDate = dt;
      dt = dt.addSecs(300);
      endDate = dt;
      break;
    case Minute10:
      tint = dt.time().minute() / 10;
      dt.setTime(QTime(dt.time().hour(), tint * 10, 0, 0));
      startDate = dt;
      dt = dt.addSecs(600);
      endDate = dt;
      break;
    case Minute15:
      tint = dt.time().minute() / 15;
      dt.setTime(QTime(dt.time().hour(), tint * 15, 0, 0));
      startDate = dt;
      dt = dt.addSecs(900);
      endDate = dt;
      break;
    case Minute30:
      tint = dt.time().minute() / 30;
      dt.setTime(QTime(dt.time().hour(), tint * 30, 0, 0));
      startDate = dt;
      dt = dt.addSecs(1800);
      endDate = dt;
      break;
    case Minute60:
      dt.setTime(QTime(dt.time().hour(), 0, 0, 0));
      startDate = dt;
      dt = dt.addSecs(3600);
      endDate = dt;
      break;
    case DailyBar:
      dt.setTime(QTime(0, 0, 0, 0));
      startDate = dt;
      dt.setTime(QTime(23, 59, 59, 0));
      endDate = dt;
      break;
    case WeeklyBar:
      dt.setTime(QTime(0, 0, 0, 0));
      dt = dt.addDays(- dt.date().dayOfWeek());
      startDate = dt;
      dt.setTime(QTime(23, 59, 59, 0));
      dt = dt.addDays(6);
      endDate = dt;
      break;
    case MonthlyBar:
      dt.setTime(QTime(0, 0, 0, 0));
      dt = dt.addDays(- (dt.date().day() - 1));
      startDate = dt;
      dt.setTime(QTime(23, 59, 59, 0));
      dt = dt.addDays(dt.date().daysInMonth() - 1);
      endDate = dt;
      break;
    default:
      break;
  }
}

void BarData::getSymbol (QString &d)
{
  d = symbol;
}
