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

#include "TradeItem.h"
#include <qobject.h>

TradeItem::TradeItem ()
{
  tradePosition = Long;
  enterSignal = None;
  exitSignal = None;
  enterPrice = 0;
  exitPrice = 0;
  volume = 0;
  profit = 0;
  stockFlag = TRUE;
  commissionType = FALSE;
  entryCom = 0;
  exitCom = 0;
  balance = 0;
  margin = 0;
}

TradeItem::~TradeItem ()
{
}

void TradeItem::setTradePosition (TradeItem::TradePosition d)
{
  tradePosition = d;
}

TradeItem::TradePosition TradeItem::getTradePosition ()
{
  return tradePosition;
}

void TradeItem::setEnterSignal (TradeItem::TradeSignal d)
{
  enterSignal = d;
}

TradeItem::TradeSignal TradeItem::getEnterSignal ()
{
  return enterSignal;
}

void TradeItem::setExitSignal (TradeItem::TradeSignal d)
{
  exitSignal = d;
}

void TradeItem::setExitSignal (QString &d)
{
  while (1)
  {
    if (! d.compare(QObject::tr("Enter Long")))
    {
      exitSignal = EnterLong;
      break;
    }

    if (! d.compare(QObject::tr("Exit Long")))
    {
      exitSignal = ExitLong;
      break;
    }


    if (! d.compare(QObject::tr("Enter Short")))
    {
      exitSignal = EnterShort;
      break;
    }

    if (! d.compare(QObject::tr("Exit Short")))
    {
      exitSignal = ExitShort;
      break;
    }

    if (! d.compare(QObject::tr("Max Loss")))
    {
      exitSignal = MaximumLoss;
      break;
    }

    if (! d.compare(QObject::tr("Profit")))
    {
      exitSignal = Profit;
      break;
    }

    if (! d.compare(QObject::tr("Trailing")))
    {
      exitSignal = Trailing;
      break;
    }

    if (! d.compare(QObject::tr("CUS Stop")))
    {
      exitSignal = CUSStop;
      break;
    }

    if (! d.compare(QObject::tr("End of test")))
    {
      exitSignal = EndTest;
      break;
    }

    break;
  }
}

TradeItem::TradeSignal TradeItem::getExitSignal ()
{
  return exitSignal;
}

void TradeItem::setEnterDate (QDateTime &d)
{
  enterDate = d;
}

void TradeItem::getEnterDate (QDateTime &d)
{
  d = enterDate;
}

void TradeItem::setExitDate (QDateTime &d)
{
  exitDate = d;
}

void TradeItem::getExitDate (QDateTime &d)
{
  d = exitDate;
}

void TradeItem::getEnterDateString (QString &s)
{
  s = enterDate.toString("yyyy-MM-dd hh:mm:ss");
}

void TradeItem::getExitDateString (QString &s)
{
  s = exitDate.toString("yyyy-MM-dd hh:mm:ss");
}

void TradeItem::setEnterPrice (double d)
{
  enterPrice = d;
}

double TradeItem::getEnterPrice ()
{
  return enterPrice;
}

void TradeItem::setExitPrice (double d)
{
  exitPrice = d;
}

double TradeItem::getExitPrice ()
{
  return exitPrice;
}

void TradeItem::setVolume (int d)
{
  volume = d;
}

int TradeItem::getVolume ()
{
  return volume;
}

void TradeItem::setStockFlag (bool d)
{
  stockFlag = d;
}

bool TradeItem::getStockFlag ()
{
  return stockFlag;
}

void TradeItem::setFuturesType (QString &d)
{
  futuresType = d;
  fd.setSymbol(futuresType);
}

void TradeItem::calculateProfit ()
{
  profit = 0;
  if (tradePosition == Short)
    profit = enterPrice - exitPrice;
  else
    profit = exitPrice - enterPrice;

  if (! stockFlag)
  {
    if (! futuresType.length())
      return;

    profit = fd.getRate() * profit;
  }

  profit = profit * volume;

  if (commissionType)
  {
    if (stockFlag)
    {
      entryCom = (enterPrice * volume) * entryCom;
      exitCom = (exitPrice * volume) * exitCom;
    }
    else
    {
      if (! futuresType.length())
        return;

      entryCom = (margin * volume) * entryCom;
      exitCom = (margin * volume) * exitCom;
    }
  }

  balance = balance + profit - entryCom - exitCom;
}

void TradeItem::getTradePositionString (QString &s)
{
  s = "L";
  if (tradePosition == Short)
    s = "S";
}

void TradeItem::getExitSignalString (QString &s)
{
  s.truncate(0);

  switch (exitSignal)
  {
    case EnterLong:
      s = QObject::tr("Enter Long");
      break;
    case ExitLong:
      s = QObject::tr("Exit Long");
      break;
    case EnterShort:
      s = QObject::tr("Enter Short");
      break;
    case ExitShort:
      s = QObject::tr("Exit Short");
      break;
    case MaximumLoss:
      s = QObject::tr("Max Loss");
      break;
    case Profit:
      s = QObject::tr("Profit");
      break;
    case Trailing:
      s = QObject::tr("Trailing");
      break;
    case CUSStop:
      s = QObject::tr("CUS Stop");
      break;
    case EndTest:
      s = QObject::tr("End of test");
      break;
    default:
      break;
  }
}

double TradeItem::getProfit ()
{
  return profit;
}

void TradeItem::setBalance (double d)
{
  balance = d;
}

double TradeItem::getBalance ()
{
  return balance;
}

void TradeItem::setEntryCom (double d)
{
  entryCom = d;
}

double TradeItem::getEntryCom ()
{
  return entryCom;
}

void TradeItem::setExitCom (double d)
{
  exitCom = d;
}

double TradeItem::getExitCom ()
{
  return exitCom;
}

void TradeItem::setCommissionType (bool d)
{
  commissionType = d;
}

bool TradeItem::getCommissionType ()
{
  return commissionType;
}

double TradeItem::getCurrentProfit (double xp)
{
  double prof = 0;
  if (tradePosition == TradeItem::Short)
    prof = enterPrice - xp;
  else
    prof = xp - enterPrice;

  if (! stockFlag)
  {
    if (! futuresType.length())
      return 0;

    prof = fd.getRate() * prof;
  }

  prof = prof * volume;

  return prof;
}

void TradeItem::setMargin (int d)
{
  margin = d;
}
