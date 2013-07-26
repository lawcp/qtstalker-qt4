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

#include "Bar.h"

Bar::Bar ()
{
  clear();
}

Bar::~Bar ()
{
}

int Bar::setDate (QDateTime &d)
{
  if (! d.isValid())
    return TRUE;

  date = d;
  return FALSE;
}

int Bar::setDate (QString &d)
{
  QString s = d;
  while (s.contains("-"))
    s = s.remove(s.find("-", 0, TRUE), 1);

  while (s.contains(":"))
    s = s.remove(s.find(":", 0, TRUE), 1);

  while (s.contains(" "))
    s = s.remove(s.find(" ", 0, TRUE), 1);

  if (s.length() != 14)
  {
    qDebug("Bar::setDate:bad string length %i", s.length());
    return TRUE;
  }

  QDate dt = QDate(s.left(4).toInt(), s.mid(4, 2).toInt(), s.mid(6, 2).toInt());
  if (! dt.isValid())
  {
    qDebug("Bar::setDate: invalid date %s", s.latin1());
    return TRUE;
  }

  int hour = s.mid(8, 2).toInt();
  if (hour < 0 || hour > 23)
  {
    qDebug("Bar::setDate: hour out of range %i", hour);
    return TRUE;
  }

  int min = s.mid(10, 2).toInt();
  if (min < 0 || min > 59)
  {
    qDebug("Bar::setDate: minute out of range %i", min);
    return TRUE;
  }

  int sec = s.right(2).toInt();
  if (sec < 0 || sec > 59)
  {
    qDebug("Bar::setDate: second out of range %i", min);
    return TRUE;
  }

  QTime t(hour, min, sec, 0);
  if (! t.isValid())
  {
    qDebug("Bar::setDate: invalid time");
    return TRUE;
  }

  date.setDate(dt);
  date.setTime(t);

  return FALSE;
}

void Bar::getDate (QDateTime &d)
{
  d = date;
}

void Bar::setOpen (double d)
{
  open = d;
  openFlag = TRUE;
  emptyFlag = FALSE;
}

double Bar::getOpen ()
{
  return open;
}

void Bar::setHigh (double d)
{
  high = d;
  highFlag = TRUE;
  emptyFlag = FALSE;
}

double Bar::getHigh ()
{
  return high;
}

void Bar::setLow (double d)
{
  low = d;
  lowFlag = TRUE;
  emptyFlag = FALSE;
}

double Bar::getLow ()
{
  return low;
}

void Bar::setClose (double d)
{
  close = d;
  closeFlag = TRUE;
  emptyFlag = FALSE;
}

double Bar::getClose ()
{
  return close;
}

void Bar::setVolume (double d)
{
  volume = d;
  volumeFlag = TRUE;
  emptyFlag = FALSE;
}

double Bar::getVolume ()
{
  return volume;
}

void Bar::setOI (int d)
{
  oi = d;
  oiFlag = TRUE;
  emptyFlag = FALSE;
}

double Bar::getOI ()
{
  return oi;
}

void Bar::getString (QString &s)
{
  getDateTimeString(TRUE, s);

  if (openFlag)
  {
    s.append(" ");
    s.append(QString::number(open, 'g'));
  }

  if (highFlag)
  {
    s.append(" ");
    s.append(QString::number(high, 'g'));
  }

  if (lowFlag)
  {
    s.append(" ");
    s.append(QString::number(low, 'g'));
  }

  if (closeFlag)
  {
    s.append(" ");
    s.append(QString::number(close, 'g'));
  }

  if (volumeFlag)
  {
    s.append(" ");
    s.append(QString::number(volume, 'g'));
  }

  if (oiFlag)
  {
    s.append(" ");
    s.append(QString::number(oi));
  }
}

bool Bar::getTickFlag ()
{
  return tickFlag;
}

void Bar::setTickFlag (bool d)
{
  tickFlag = d;
}

bool Bar::getEmptyFlag ()
{
  return emptyFlag;
}

void Bar::setEmptyFlag (bool d)
{
  emptyFlag = d;
}

void Bar::getDateString (bool sepFlag, QString &d)
{
  if (sepFlag)
    d = date.toString("yyyy-MM-dd");
  else
    d = date.toString("yyyyMMdd");
}

void Bar::getDateTimeString (bool sepFlag, QString &d)
{
  QString s;
  getDateString(sepFlag, s);

  if (sepFlag)
    s.append(" ");

  QString s2;
  getTimeString(sepFlag, s2);
  s.append(s2);
  d = s;
}

void Bar::getTimeString (bool sepFlag, QString &d)
{
  if (sepFlag)
    d = date.toString("hh:mm:ss");
  else
    d = date.toString("hhmmss");
}

void Bar::clear ()
{
  date = QDateTime::currentDateTime();
  date.setTime(QTime(0,0,0,0));

  tickFlag = FALSE;
  open = 0;
  high = 0;
  low = 0;
  close = 0;
  volume = 0;
  oi = 0;
  openFlag = FALSE;
  highFlag = FALSE;
  lowFlag = FALSE;
  closeFlag = FALSE;
  volumeFlag = FALSE;
  oiFlag = FALSE;
  emptyFlag = TRUE;
}

bool Bar::verify ()
{
  bool rc = TRUE;

  if (open == 0 || high == 0 || low == 0 || close == 0)
    return rc;

  if (open > high)
    open = high;
  if (open < low)
    open = low;

  if (close > high)
    close = high;
  if (close < low)
    close = low;

  rc = FALSE;
  return rc;
}
