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

#include "FuturesData.h"
#include <qobject.h>
#include <qfileinfo.h>

FuturesData::FuturesData ()
{
  monthList.append("F");
  monthList.append("G");
  monthList.append("H");
  monthList.append("J");
  monthList.append("K");
  monthList.append("M");
  monthList.append("N");
  monthList.append("Q");
  monthList.append("U");
  monthList.append("V");
  monthList.append("X");
  monthList.append("Z");

  symbolList.append("AD");
  symbolList.append("BO");
  symbolList.append("NB");
  symbolList.append("BP");
  symbolList.append("C");
  symbolList.append("CC");
  symbolList.append("CO");
  symbolList.append("CD");
  symbolList.append("CL");
  symbolList.append("CR");
  symbolList.append("CT");
  symbolList.append("DJ");
  symbolList.append("DX");
  symbolList.append("EC");
  symbolList.append("ED");
  symbolList.append("ES");
  symbolList.append("FC");
  symbolList.append("GC");
  symbolList.append("GI");
  symbolList.append("HG");
  symbolList.append("HO");
  symbolList.append("HU");
  symbolList.append("OJ");
  symbolList.append("JO");
  symbolList.append("JY");
  symbolList.append("KC");
  symbolList.append("LB");
  symbolList.append("LC");
  symbolList.append("LN");
  symbolList.append("LH");
  symbolList.append("ND");
  symbolList.append("NG");
  symbolList.append("NQ");
  symbolList.append("O");
  symbolList.append("PA");
  symbolList.append("PB");
  symbolList.append("PL");
  symbolList.append("S");
  symbolList.append("SB");
  symbolList.append("SF");
  symbolList.append("SI");
  symbolList.append("SM");
  symbolList.append("SP");
  symbolList.append("TY");
  symbolList.append("TYD");
  symbolList.append("US");
  symbolList.append("USD");
  symbolList.append("W");
  symbolList.append("YX");
  symbolList.append("FW20");

  cme = "CME";
  cbot = "CBOT";
  nymex = "NYMEX";
  nybot = "NYBOT";
  hmuz = "H,M,U,Z";
  hknuz = "H,K,N,U,Z";
  all = "F,G,H,J,K,M,N,Q,U,V,X,Z";
}

FuturesData::~FuturesData ()
{
}

void FuturesData::getName (QString &d)
{
  d = name;
}

void FuturesData::getSymbol (QString &d)
{
  d = symbol;
}

void FuturesData::getExchange (QString &d)
{
  d = exchange;
}

void FuturesData::getContract (QString &d)
{
  d = contract;
}

double FuturesData::getLimit ()
{
  return limit;
}

double FuturesData::getRate ()
{
  return rate;
}

void FuturesData::getSymbolList (QStringList &l)
{
  l = symbolList;
}

void FuturesData::getCMESymbolList (QStringList &l)
{
  l.clear();
  l.append("AD");
  l.append("NB");
  l.append("CD");
  l.append("EC");
  l.append("ED");
  l.append("ES");
  l.append("FC");
  l.append("GI");
  l.append("JY");
  l.append("LB");
  l.append("LC");
  l.append("LN");
  l.append("ND");
  l.append("NQ");
  l.append("PB");
  l.append("SF");
  l.append("SP");
}

void FuturesData::getMonthList (QStringList &l)
{
  l.clear();
  l = QStringList::split(",", month, FALSE);
}

void FuturesData::getMonths (QStringList &l)
{
  l = monthList;
}

int FuturesData::setSymbol (QString &d)
{
  name.truncate(0);
  month.truncate(0);
  exchange.truncate(0);
  symbol.truncate(0);
  rate = 0;
  limit = 0;

  bool rc = TRUE;
  int i = symbolList.findIndex(d);
  switch (i)
  {
    case AD:
      name = QObject::tr("Australian Dollar");
      rate = 1000;
      month = hmuz;
      exchange = cme;
      symbol = "AD";
      rc = FALSE;
      break;
    case BO:
      name = QObject::tr("Soybean Oil");
      rate = 600;
      month = "F,H,K,N,Q,U,V,Z";
      limit = 2;
      exchange = cbot;
      symbol = "BO";
      rc = FALSE;
      break;
    case BP:
    case NB:
      name = QObject::tr("British Pound");
      rate = 625;
      month = hmuz;
      exchange = cme;
      symbol = "NB";
      rc = FALSE;
      break;
    case C:
      name = QObject::tr("Corn");
      rate = 50;
      month = hknuz;
      limit = 20;
      exchange = cbot;
      symbol = "C";
      rc = FALSE;
      break;
    case CC:
    case CO:
      name = QObject::tr("Cocoa");
      rate = 10;
      month = hknuz;
      exchange = nybot;
      symbol = "CC";
      rc = FALSE;
      break;
    case CD:
      name = QObject::tr("Canadian Dollar");
      rate = 1000;
      month = hmuz;
      exchange = cme;
      symbol = "CD";
      rc = FALSE;
      break;
    case CL:
      name = QObject::tr("Crude Oil");
      rate = 1000;
      month = all;
      limit = 3;
      exchange = nymex;
      symbol = "CL";
      rc = FALSE;
      break;
    case CR:
      name = QObject::tr("CRB Index");
      month = "F,G,J,M,Q,X";
      exchange = nybot;
      symbol = "CR";
      rc = FALSE;
      break;
    case CT:
      name = QObject::tr("Cotton");
      rate = 500;
      month = "H,K,N,V,Z";
      limit = 3;
      exchange = nybot;
      symbol = "CT";
      rc = FALSE;
      break;
    case DJ:
      name = QObject::tr("Dow Jones Industrial Average");
      month = hmuz;
      exchange = cbot;
      symbol = "DJ";
      rc = FALSE;
      break;
    case DX:
      name = QObject::tr("US Dollar Index");
      rate = 1000;
      month = hmuz;
      exchange = nybot;
      symbol = "DX";
      rc = FALSE;
      break;
    case EC:
      name = QObject::tr("Euro");
      month = hmuz;
      exchange = cme;
      symbol = "EC";
      rc = FALSE;
      break;
    case ED:
      name = QObject::tr("Eurodollar");
      rate = 2500;
      month = hmuz;
      exchange = cme;
      symbol = "ED";
      rc = FALSE;
      break;
    case ES:
      name = QObject::tr("E-MINI S&P 500");
      month = hmuz;
      exchange = cme;
      rate = 50;
      symbol = "ES";
      rc = FALSE;
      break;
    case FC:
      name = QObject::tr("Feeder Cattle");
      rate = 500;
      month = "F,H,J,K,Q,U,V,X";
      limit = 1.5;
      exchange = cme;
      symbol = "FC";
      rc = FALSE;
      break;
    case GC:
      name = QObject::tr("Gold");
      rate = 100;
      month = "G,J,M,Q,V,Z";
      limit = 75;
      symbol = "GC";
      exchange = nymex;
      rc = FALSE;
      break;
    case GI:
      name = QObject::tr("Goldman Sachs Commodity Index");
      month = all;
      symbol = "GI";
      exchange = cme;
      rc = FALSE;
      break;
    case HG:
      name = QObject::tr("Copper");
      rate = 250;
      month = hknuz;
      limit = 20;
      symbol = "HG";
      exchange = nymex;
      rc = FALSE;
      break;
    case HO:
      name = QObject::tr("Heating Oil");
      rate = 40000;
      month = all;
      limit = 6;
      symbol = "HO";
      exchange = nymex;
      rc = FALSE;
      break;
    case HU:
      name = QObject::tr("Unleaded Gasoline");
      rate = 40000;
      month = all;
      limit = 6;
      symbol = "HU";
      exchange = nymex;
      rc = FALSE;
      break;
    case OJ:
    case JO:
      name = QObject::tr("Frozen Concentrated Orange Juice");
      rate = 150;
      month = "F,H,K,N,U,X";
      limit = 5;
      symbol = "OJ";
      exchange = nybot;
      rc = FALSE;
      break;
    case JY:
      symbol = QObject::tr("Japanese Yen");
      rate = 1250;
      month = hmuz;
      symbol = "JY";
      exchange = cme;
      rc = FALSE;
      break;
    case KC:
      name = QObject::tr("Coffee");
      rate = 375;
      month = hknuz;
      symbol = "KC";
      exchange = nybot;
      rc = FALSE;
      break;
    case LB:
      name = QObject::tr("Lumber");
      rate = 80;
      month = "F,H,K,N,U,X";
      limit = 10;
      exchange = cme;
      symbol = "LB";
      rc = FALSE;
      break;
    case LC:
      name = QObject::tr("Live Cattle");
      rate = 400;
      month = "G,J,M,Q,V,Z";
      limit = 1.5;
      exchange = cme;
      symbol = "LC";
      rc = FALSE;
      break;
    case LH:
    case LN:
      name = QObject::tr("Lean Hogs");
      rate = 400;
      month = "G,J,M,N,Q,V,Z";
      limit = 2;
      symbol = "LN";
      exchange = cme;
      rc = FALSE;
      break;
    case ND:
      name = QObject::tr("NASDAQ 100");
      month = hmuz;
      symbol = "ND";
      exchange = cme;
      rc = FALSE;
      break;
    case NG:
      name = QObject::tr("Natural Gas");
      rate = 10000;
      month = all;
      limit = 1;
      symbol = "NG";
      exchange = nymex;
      rc = FALSE;
      break;
    case NQ:
      name = QObject::tr("E-MINI NASDAQ 100");
      month = hmuz;
      symbol = "NQ";
      exchange = cme;
      rc = FALSE;
      break;
    case O:
      name = QObject::tr("Oats");
      rate = 50;
      month = hknuz;
      limit = 20;
      symbol = "O";
      exchange = cbot;
      rc = FALSE;
      break;
    case PA:
      name = QObject::tr("Palladium");
      rate = 100;
      month = hmuz;
      symbol = "PA";
      exchange = nymex;
      rc = FALSE;
      break;
    case PB:
      name = QObject::tr("Frozen Pork Bellies");
      rate = 400;
      month = "G,H,K,N,Q";
      limit = 3;
      symbol = "PB";
      exchange = cme;
      rc = FALSE;
      break;
    case PL:
      name = QObject::tr("Platinum");
      rate = 50;
      month = "F,J,N,V";
      limit = 50;
      symbol = "PL";
      exchange = nymex;
      rc = FALSE;
      break;
    case S:
      name = QObject::tr("Soybeans");
      rate = 50;
      limit = 50;
      month = "F,H,K,N,Q,U,X";
      symbol = "S";
      exchange = cbot;
      rc = FALSE;
      break;
    case SB:
      name = QObject::tr("Sugar #11 World");
      rate = 1120;
      month = "H,K,N,V";
      symbol = "SB";
      exchange = nybot;
      rc = FALSE;
      break;
    case SF:
      name = QObject::tr("Swiss Franc");
      rate = 1250;
      month = hmuz;
      symbol = "SF";
      exchange = cme;
      rc = FALSE;
      break;
    case SI:
      name = QObject::tr("Silver");
      rate = 100;
      month = "F,H,K,N,U,Z";
      rate = 150;
      symbol = "SI";
      exchange = nymex;
      rc = FALSE;
      break;
    case SM:
      name = QObject::tr("Soy Meal");
      rate = 100;
      month = "F,H,K,N,Q,U,V,Z";
      limit = 20;
      symbol = "SM";
      exchange = cbot;
      rc = FALSE;
      break;
    case SP:
      name = QObject::tr("S&P 500");
      month = hmuz;
      exchange = cme;
      rate = 250;
      symbol = "SP";
      rc = FALSE;
      break;
    case TY:
    case TYD:
      name = QObject::tr("Treasury Note 10 yr.");
      rate = 1000;
      month = hmuz;
      symbol = "TY";
      exchange = cbot;
      rc = FALSE;
      break;
    case US:
    case USD:
      name = QObject::tr("US Treasury Bond");
      rate = 1000;
      month = hmuz;
      symbol = "US";
      exchange = cbot;
      rc = FALSE;
      break;
    case W:
      name = QObject::tr("Wheat");
      rate = 50;
      month = hknuz;
      limit = 30;
      symbol = "W";
      exchange = cbot;
      rc = FALSE;
      break;
    case YX:
      name = QObject::tr("NYSE");
      month = hmuz;
      symbol = "YX";
      exchange = nybot;
      rc = FALSE;
      break;
    case FW20:
      // Warsaw Stock Exchange futures
      name = QObject::tr("WIG20 Index");
      month = hmuz;
      exchange = "WSE";
      rate = 10;
      symbol = "FW20";
      rc = FALSE;
      break;
    default:
      break;
  }

  if (rc)
    return rc;

  QDateTime dt = QDateTime::currentDateTime();
  getCurrentContract(dt, contract);

  return rc;
}

void FuturesData::getCurrentContract (QDateTime &dt, QString &cont)
{
  cont = symbol;
  bool yearFlag = FALSE;
  QStringList ml;
  getMonthList(ml);
  QStringList fml;
  getMonths(fml);

  int currentMonth = dt.date().month() - 1;

  int i = ml.findIndex(fml[currentMonth]);
  if (i != -1)
  {
    currentMonth++;
    if (currentMonth == 12)
    {
      yearFlag = TRUE;
      currentMonth = 0;
    }
  }

  if (! symbol.compare("CL") ||
      ! symbol.compare("HO") ||
      ! symbol.compare("HU") ||
      ! symbol.compare("NG"))
  {
    currentMonth++;
    if (currentMonth == 12)
    {
      yearFlag = TRUE;
      currentMonth = 0;
    }
  }

  while (1)
  {
    QString s = fml[currentMonth];
    int i = ml.findIndex(s);
    if (i != -1)
    {
      if (yearFlag)
        cont.append(QString::number(dt.date().year() + 1));
      else
        cont.append(QString::number(dt.date().year()));

      cont.append(fml[currentMonth]);
      break;
    }
    else
    {
      currentMonth++;
      if (currentMonth == 12)
      {
        yearFlag = TRUE;
        currentMonth = 0;
      }
    }
  }
}

int FuturesData::setSymbolPath (QString &d)
{
  QFileInfo fi(d);
  QString s = fi.fileName();
  if (s.length() == 7)
    s = s.left(2);
  else
    s = s.left(1);
  return setSymbol(s);
}
