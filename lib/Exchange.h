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

#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

#include <qstring.h>

class Exchange
{
  public:

    enum DbType
    {
      CBOT,
      CME,
      NYBOT,
      NYMEX,
      NYSE,
      NASDAQ,
      AMEX,
      BASE, // Buenos Aires
      VASE, // Vienna
      AXSE, // Australia
      SASE, // Sao Paolo
      TOSE, // Toronto
      TSXVE, // Vancouver
      SSSE, // Shanghai
      SZSE, // Shenzhen
      COSE, // Copenhagen
      PASE, // Paris
      BESE, // Berlin
      BMSE, // Bremen
      DUSE, // Dusseldorf
      FSE, // Frankfurt
      HMSE, // Hamburg
      HASE, // Hanover
      MUSE, // Munich
      SGSE, // Stuttgart
      XETRA, // XETRA
      HKSE, // Hong Kong
      BOSE, // Bombay
      NSEOI, // National Stock Exchange of India
      JKSE, // Jakarta
      TASE, // Tel Aviv
      MISE, // Milan
      KSSE, // Korea
      KOSDAQ, // Korea
      MXSE, // Mexico
      ASSE, // Amsterdam
      NZSE, // New Zealand
      OLSE, // Oslo
      SISE, // Singapore
      BCSE, // Barcelona
      BISE, // Bilbao
      MFIM, // Madrid Fixed Income Market
      MCCATS, // Madrid SE C.A.T.S
      MASE, // Madrid
      STSE, // Stockholm
      SWE, // Swiss Exchange
      TWOOTC, // Taiwan OTC
      TWSE, // Taiwan
      LSE // London
    };

    Exchange();
    ~Exchange();
    void getExchange(int, QString &);

  protected:
};

#endif
