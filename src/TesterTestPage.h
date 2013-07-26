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

#ifndef TESTERTESTPAGE_HPP
#define TESTERTESTPAGE_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include "SymbolButton.h"


class TesterTestPage : public QWidget
{
  Q_OBJECT

  public:
    TesterTestPage (QWidget *);
    ~TesterTestPage ();
    bool getTradeLong ();
    void setTradeLong (bool);
    bool getTradeShort ();
    void setTradeShort (bool);
    int getVolumePercent ();
    void setVolumePercent (int);
    double getEntryCom ();
    void setEntryCom (double);
    double getExitCom ();
    void setExitCom (double);
    int getTradeDelay ();
    void setTradeDelay (int);
    QString getPriceField ();
    void setPriceField (QString);
    int getBars ();
    void setBars (int);
    QString getSymbol ();
    QString getSymbolPath ();
    void setSymbol (QString);
    int getAccount ();
    void setAccount (int);
    QString getBarLength ();
    int getBarLengthIndex ();
    void setBarLength (QString);
    int getMargin ();
    void setMargin (int);
    bool getCommissionType ();
    void setCommissionType (bool);

  public slots:
    void symbolButtonPressed ();

  protected:
    QCheckBox *tradeLong;
    QCheckBox *tradeShort;
    QCheckBox *commissionType;
    SymbolButton *symbolButton;
    QLineEdit *entryCom;
    QLineEdit *exitCom;
    QSpinBox *account;
    QSpinBox *volumePercent;
    QSpinBox *margin;
    QSpinBox *tradeDelay;
    QSpinBox *bars;
    QComboBox *priceField;
    QComboBox *barLength;
    QStringList fieldList;
    QStringList barLengthList;
};

#endif
