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

#include "IndicatorPlugin.h"
//Added by qt3to4:
#include <Q3PtrList>

class PP : public IndicatorPlugin
{
  public:
    PP();
    virtual ~PP();
    Indicator * calculate();
    int indicatorPrefDialog(QWidget *);
    void setDefaults();
    void getIndicatorSettings(Setting &);
    void setIndicatorSettings(Setting &);
    PlotLine * calculateCustom(QString &, Q3PtrList<PlotLine> &);
    void formatDialog(QStringList &vl, QString &rv, QString &rs);
    void getPP(Q3PtrList<PlotLine> &);

  private:
    QColor resColor;
    QColor supColor;
    PlotLine::LineType resLineType;
    PlotLine::LineType supLineType;
    QString resLabel;
    QString resLabel2;
    QString resLabel3;
    QString supLabel;
    QString supLabel2;
    QString supLabel3;
    QString label;
    QStringList ppList;

    QString resColorLabel;
    QString supColorLabel;
    QString resLineTypeLabel;
    QString supLineTypeLabel;
    QString resLabelLabel;
    QString resLabel2Label;
    QString resLabel3Label;
    QString supLabelLabel;
    QString supLabel2Label;
    QString supLabel3Label;
    QString labelLabel;
    QString pluginLabel;
};
