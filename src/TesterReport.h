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

#ifndef TESTERREPORT_HPP
#define TESTERREPORT_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qlabel.h>
#include <q3table.h>
//Added by qt3to4:
#include <Q3PtrList>
#include "TradeItem.h"


class TesterReport : public QWidget
{
  Q_OBJECT

  public:
    TesterReport (QWidget *);
    ~TesterReport ();
    void getSummary (QStringList &);
    void createSummary (Q3PtrList<TradeItem> &, double account);
    void addTrade (QString &, TradeItem *);
    void clear ();

  protected:
    Q3Table *tradeTable;
    QLabel *summaryBalance;
    QLabel *summaryNetProfit;
    QLabel *summaryNetPercentage;
    QLabel *summaryInvestment;
    QLabel *summaryCommission;
    QLabel *summaryDrawdown;
    QLabel *summaryLongTrades;
    QLabel *summaryShortTrades;
    QLabel *summaryTrades;
    QLabel *summaryWinTrades;
    QLabel *summaryTotalWinTrades;
    QLabel *summaryAverageWin;
    QLabel *summaryLargestWin;
    QLabel *summaryWinLongTrades;
    QLabel *summaryWinShortTrades;
    QLabel *summaryLoseTrades;
    QLabel *summaryTotalLoseTrades;
    QLabel *summaryAverageLose;
    QLabel *summaryLargestLose;
    QLabel *summaryLoseLongTrades;
    QLabel *summaryLoseShortTrades;
};

#endif
