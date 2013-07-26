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

#include "Exchange.h"
#include <qobject.h>

Exchange::Exchange ()
{
}

Exchange::~Exchange ()
{
}

void Exchange::getExchange (int i, QString &d)
{
  switch (i)
  {
    case CBOT:
      d = "CBOT";
      break;
    case CME:
      d = "CME";
      break;
    case NYBOT:
      d = "NYBOT";
      break;
    case NYMEX:
      d = "NYMEX";
      break;
    case NYSE:
      d = "NYSE";
      break;
    case NASDAQ:
      d = "NASDAQ";
      break;
    case AMEX:
      d = "AMEX";
      break;
    case BASE:
      d = QObject::tr("Buenos Aires Stock Exchange");
      break;
    case VASE:
      d = QObject::tr("Vienna Stock Exchange");
      break;
    case AXSE:
      d = QObject::tr("Australian Stock Exchange");
      break;
    case SASE:
      d = QObject::tr("Sao Paolo Stock Exchange");
      break;
    case TOSE:
      d = QObject::tr("Toronto Stock Exchange");
      break;
    case TSXVE:
      d = QObject::tr("TSX Venture Exchange");
      break;
    case SSSE:
      d = QObject::tr("Shanghai Stock Exchange");
      break;
    case SZSE:
      d = QObject::tr("Shenzhen Stock Exchange");
      break;
    case COSE:
      d = QObject::tr("Copenhagen Stock Exchange");
      break;
    case PASE:
      d = QObject::tr("Paris Stock Exchange");
      break;
    case BESE:
      d = QObject::tr("Berlin Stock Exchange");
      break;
    case BMSE:
      d = QObject::tr("Bremen Stock Exchange");
      break;
    case DUSE:
      d = QObject::tr("Dusseldorf Stock Exchange");
      break;
    case FSE:
      d = QObject::tr("Frankfurt Stock Exchange");
      break;
    case HMSE:
      d = QObject::tr("Hamburg Stock Exchange");
      break;
    case HASE:
      d = QObject::tr("Hanover Stock Exchange");
      break;
    case MUSE:
      d = QObject::tr("Munich Stock Exchange");
      break;
    case SGSE:
      d = QObject::tr("Stuttgart Stock Exchange");
      break;
    case XETRA:
      d = QObject::tr("XETRA Stock Exchange");
      break;
    case HKSE:
      d = QObject::tr("Hong Kong Stock Exchange");
      break;
    case BOSE:
      d = QObject::tr("Bombay Stock Exchange");
      break;
    case NSEOI:
      d = QObject::tr("National Stock Exchange of India");
      break;
    case JKSE:
      d = QObject::tr("Jakarta Stock Exchange");
      break;
    case TASE:
      d = QObject::tr("Tel Aviv Stock Exchange");
      break;
    case MISE:
      d = QObject::tr("Milan Stock Exchange");
      break;
    case KSSE:
      d = QObject::tr("Korea Stock Exchange");
      break;
    case KOSDAQ:
      d = QObject::tr("KOSDAQ");
      break;
    case MXSE:
      d = QObject::tr("Mexico Stock Exchange");
      break;
    case ASSE:
      d = QObject::tr("Amsterdam Stock Exchange");
      break;
    case NZSE:
      d = QObject::tr("New Zealand Stock Exchange");
      break;
    case OLSE:
      d = QObject::tr("Oslo Stock Exchange");
      break;
    case SISE:
      d = QObject::tr("Singapore Stock Exchange");
      break;
    case BCSE:
      d = QObject::tr("Barcelona Stock Exchange");
      break;
    case BISE:
      d = QObject::tr("Bilbao Stock Exchange");
      break;
    case MFIM:
      d = QObject::tr("Madrid Fixed Income Market");
      break;
    case MCCATS:
      d = QObject::tr("Madrid SE C.A.T.S.");
      break;
    case MASE:
      d = QObject::tr("Madrid Stock Exchange");
      break;
    case STSE:
      d = QObject::tr("Stockholm Stock Exchange");
      break;
    case SWE:
      d = QObject::tr("Swiss Exchange");
      break;
    case TWOOTC:
      d = QObject::tr("Taiwan OTC Exchange");
      break;
    case TWSE:
      d = QObject::tr("Taiwan Stock Exchange");
      break;
    case LSE:
      d = QObject::tr("London Stock Exchange");
      break;
    default:
      break;
  }
}
