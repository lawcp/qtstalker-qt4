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

#ifndef FUTURESDATA_HPP
#define FUTURESDATA_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>

class FuturesData
{
  public:
    enum FuturesSymbol
    {
      AD,
      BO,
      NB,
      BP,
      C,
      CC,
      CO,
      CD,
      CL,
      CR,
      CT,
      DJ,
      DX,
      EC,
      ED,
      ES,
      FC,
      GC,
      GI,
      HG,
      HO,
      HU,
      OJ,
      JO,
      JY,
      KC,
      LB,
      LC,
      LN,
      LH,
      ND,
      NG,
      NQ,
      O,
      PA,
      PB,
      PL,
      S,
      SB,
      SF,
      SI,
      SM,
      SP,
      TY,
      TYD,
      US,
      USD,
      W,
      YX,
      FW20
    };

    FuturesData();
    ~FuturesData();
    void getName(QString &);
    void getSymbol(QString &);
    void getExchange(QString &);
    void getContract(QString &);
    double getLimit();
    double getRate();
    int setSymbol(QString &);
    void getMonthList(QStringList &);
    void getMonths(QStringList &);
    void getSymbolList(QStringList &);
    void getCurrentContract(QDateTime &, QString &);
    int setSymbolPath(QString &);
    void getCMESymbolList(QStringList &);

  protected:
    QString name;
    QString symbol;
    QString exchange;
    QString month;
    float rate;
    float limit;
    QStringList monthList;
    QStringList symbolList;
    QString contract;
    QString cme;
    QString cbot;
    QString nymex;
    QString nybot;
    QString hmuz;
    QString hknuz;
    QString all;
};

#endif
