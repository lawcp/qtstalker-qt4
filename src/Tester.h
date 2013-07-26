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

#ifndef TESTER_HPP
#define TESTER_HPP

#include <qstring.h>
#include <q3tabdialog.h>
#include <q3ptrlist.h>
#include <q3dict.h>
#include "Config.h"
#include "Setting.h"
#include "BarData.h"
#include "TesterReport.h"
#include "TradeItem.h"
#include "TesterRulePage.h"
#include "TesterTestPage.h"
#include "TesterStopPage.h"
#include "TesterChartPage.h"
#include "DBIndex.h"


class Tester : public Q3TabDialog
{
  Q_OBJECT

  signals:
    void message (QString);

  public:
    Tester (QString, DBIndex *);
    Tester ();
    ~Tester ();
    int getVolume (int, double);
    double getPrice (int);
    QString newTest ();
    void loadSignals ();
    void enterTrade (TradeItem::TradePosition);

  public slots:
    void test ();
    void saveRule ();
    void exitDialog ();
    void loadRule ();
    void slotHelp ();

  protected:
    Config config;
    TesterReport *reportPage;
    TesterRulePage *rulePage;
    TesterTestPage *testPage;
    TesterStopPage *stopPage;
    TesterChartPage *chartPage;
    QString ruleName;
    QString chartType;
    QString futuresType;
    BarData *recordList;
    int currentRecord;
    double equity;
    double volume;
    Q3Dict<Setting> enterLongSignal;
    Q3Dict<Setting> exitLongSignal;
    Q3Dict<Setting> enterShortSignal;
    Q3Dict<Setting> exitShortSignal;
    Q3PtrList<TradeItem> trades;
    DBIndex *index;
};

#endif
