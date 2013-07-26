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

#ifndef TRADEITEM_HPP
#define TRADEITEM_HPP

#include "BarData.h"
#include "FuturesData.h"
#include <qstring.h>
#include <qdatetime.h>

class TradeItem
{
  public:

    enum TradeSignal
    {
      None,
      EnterLong,
      ExitLong,
      EnterShort,
      ExitShort,
      MaximumLoss,
      Profit,
      Trailing,
      CUSStop,
      EndTest
    };

    enum TradePosition
    {
      Long,
      Short
    };

    TradeItem ();
    ~TradeItem ();
    void setTradePosition (TradeItem::TradePosition);
    TradeItem::TradePosition getTradePosition ();
    void setEnterSignal (TradeItem::TradeSignal);
    TradeItem::TradeSignal getEnterSignal ();
    void setExitSignal (TradeItem::TradeSignal);
    void setExitSignal (QString &);
    TradeItem::TradeSignal getExitSignal ();
    void setEnterDate (QDateTime &);
    void getEnterDate (QDateTime &);
    void setExitDate (QDateTime &);
    void getExitDate (QDateTime &);
    void getEnterDateString (QString &);
    void getExitDateString (QString &);
    void setEnterPrice (double);
    double getEnterPrice ();
    void setExitPrice (double);
    double getExitPrice ();
    void setVolume (int);
    int getVolume ();
    void setStockFlag (bool);
    bool getStockFlag ();
    void setFuturesType (QString &);
    void calculateProfit ();
    void getTradePositionString (QString &);
    void getExitSignalString (QString &);
    double getProfit ();
    double getBalance ();
    void setBalance (double);
    void setEntryCom (double);
    double getEntryCom ();
    void setExitCom (double);
    double getExitCom ();
    void setCommissionType (bool);
    bool getCommissionType ();
    double getCurrentProfit (double xp);
    void setMargin (int);

  protected:
    TradePosition tradePosition;
    TradeSignal enterSignal;
    TradeSignal exitSignal;
    QDateTime enterDate;
    QDateTime exitDate;
    double enterPrice;
    double exitPrice;
    double profit;
    double balance;
    int volume;
    bool stockFlag;
    QString futuresType;
    bool commissionType;
    double entryCom;
    double exitCom;
    int margin;
    FuturesData fd;
};

#endif
