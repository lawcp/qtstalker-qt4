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
#include <q3ptrlist.h>

class SINWAV : public IndicatorPlugin
{
  public:
    SINWAV();
    virtual ~SINWAV();
    Indicator * calculate();
    int indicatorPrefDialog(QWidget *);
    void setDefaults();
    PlotLine * calculateCustom(QString &, Q3PtrList<PlotLine> &);
    void getIndicatorSettings(Setting &);
    void setIndicatorSettings(Setting &);
    void formatDialog(QStringList &vl, QString &rv, QString &rs);
    void getSINWAV(Q3PtrList<PlotLine> &);

  private:
    QColor colorSin;
    QColor colorLead;
    PlotLine::LineType lineTypeSin;
    PlotLine::LineType lineTypeLead;
    QString labelSin;
    QString labelLead;

    PlotLine *getOffset( PlotLine *inSig, int offset );
    PlotLine *getDetrend( PlotLine *inSig );
    PlotLine *getSmooth( PlotLine *inSig );

    QString colorSinLabel;
    QString colorLeadLabel;
    QString labelSinLabel;
    QString labelLeadLabel;
    QString lineTypeSinLabel;
    QString lineTypeLeadLabel;
    QString pluginLabel;
};
