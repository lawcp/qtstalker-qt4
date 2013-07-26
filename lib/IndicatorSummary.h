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

#ifndef INDICATORSUMMARY_HPP
#define INDICATORSUMMARY_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <q3ptrlist.h>
#include <qobject.h>
#include "BarData.h"
#include "Config.h"
#include "Indicator.h"
#include "DBIndex.h"

class IndicatorSummary : public QObject
{
  Q_OBJECT

  signals:
    void signalWakeup();

  public:
    IndicatorSummary(QStringList &, int, BarData::BarLength, DBIndex *);
    ~IndicatorSummary();
    void run();
    void loadIndicators();
    void createDataWindow(Q3PtrList<Setting> &, QString &group);

  private:
    QString helpFile;

  protected:
    Config config;
    QStringList indicatorList;
    int minBars;
    BarData::BarLength barLength;
    Q3PtrList<Indicator> indicators;
    DBIndex *chartIndex;
};

#endif
