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

#ifndef TESTERSTOPPAGE_HPP
#define TESTERSTOPPAGE_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include "PlotLine.h"
#include "FormulaEdit.h"
#include "BarData.h"


class TesterStopPage : public QWidget
{
  Q_OBJECT

  public:
    TesterStopPage (QWidget *);
    ~TesterStopPage ();
    bool loadCustomLongStop (BarData *recordList);
    bool loadCustomShortStop (BarData *recordList);
    void saveCustomStopRule (QString &);
    void loadCustomStopRule (QString &);
    bool maximumLoss (bool flag, double enterPrice, double exitPrice);
    bool profit (bool flag, double enterPrice, double exitPrice);
    bool trailing (bool flag, double exitPrice);
    bool customStop (bool flag, int index);
    void setTrailingHigh (double);

    bool getMaximumLossCheck ();
    void setMaximumLossCheck (bool);
    bool getMaximumLossLong ();
    void setMaximumLossLong (bool);
    bool getMaximumLossShort ();
    void setMaximumLossShort (bool);
    QString getMaximumLossEdit ();
    void setMaximumLossEdit (QString);

    bool getProfitCheck ();
    void setProfitCheck (bool);
    bool getProfitLong ();
    void setProfitLong (bool);
    bool getProfitShort ();
    void setProfitShort (bool);
    QString getProfitEdit ();
    void setProfitEdit (QString);

    bool getTrailingCheck ();
    void setTrailingCheck (bool);
    bool getTrailingLong ();
    void setTrailingLong (bool);
    bool getTrailingShort ();
    void setTrailingShort (bool);
    QString getTrailingEdit ();
    void setTrailingEdit (QString);


  public slots:
    void maximumLossToggled (bool);
    void profitToggled (bool);
    void trailingToggled (bool);
    void customLongStopToggled (bool);
    void customShortStopToggled (bool);

  protected:
    QCheckBox *maximumLossCheck;
    QCheckBox *maximumLossLong;
    QCheckBox *maximumLossShort;
    QCheckBox *profitCheck;
    QCheckBox *profitLong;
    QCheckBox *profitShort;
    QCheckBox *trailingCheck;
    QCheckBox *trailingLong;
    QCheckBox *trailingShort;
    QCheckBox *customLongStopCheck;
    QCheckBox *customShortStopCheck;
    QLineEdit *maximumLossEdit;
    QLineEdit *profitEdit;
    QLineEdit *trailingEdit;
    QDoubleValidator *validator;
    FormulaEdit *customShortStopEdit;
    FormulaEdit *customLongStopEdit;
    double trailingHigh;
    double trailingLow;
    PlotLine *customShortStopLine;
    PlotLine *customLongStopLine;
};

#endif
