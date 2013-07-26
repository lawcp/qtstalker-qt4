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
#include "qtsFFT.h"
//Added by qt3to4:
#include <Q3PtrList>

class LOWPASS : public IndicatorPlugin
{
  public:
    LOWPASS();
    virtual ~LOWPASS();
    Indicator * calculate();
    PlotLine * getLowpass(PlotLine *in, double fre, double wid);
    int indicatorPrefDialog(QWidget *);
    void setDefaults();
    PlotLine * calculateCustom(QString &, Q3PtrList<PlotLine> &);
    void getIndicatorSettings(Setting &);
    void setIndicatorSettings(Setting &);
    void formatDialog(QStringList &vl, QString &rv, QString &rs);

  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    double freq;
    double width;
    BarData::InputType input;
    qtsFFT * fft;
    void detrend(PlotLine* inLine, double* slope, double* intercept);
    PlotLine * detrend(PlotLine *x, double &slope, double &intercept, bool detrend = true);
    PlotLine * raise2Power(PlotLine *x, double pad = 0.0);

    QString colorLabel;
    QString lineTypeLabel;
    QString labelLabel;
    QString inputLabel;
    QString pluginLabel;
    QString freqLabel;
    QString widthLabel;
};
