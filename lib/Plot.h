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

#ifndef PLOT_HPP
#define PLOT_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstringlist.h>
#include "Indicator.h"
#include "Scaler.h"
#include "BarData.h"
#include "DatePlot.h"
#include "ScalePlot.h"
#include "IndicatorPlot.h"
#include "DBIndex.h"


class Plot : public QWidget
{
  Q_OBJECT

  public:
    Plot(QWidget *, DBIndex *);
    ~Plot();
    void clear();
    void setData(BarData *);
    void setLogScale(bool);
    void setInfoFlag(bool);
    void drawCrossHair();
    void addIndicator(Indicator *);
    Indicator * getIndicator();
    bool deleteIndicator();
    void addChartObject(Setting &);
    int getWidth();
    void setGridFlag(bool);
    void setScaleToScreen(bool);
    void setDrawMode(bool);
    void setDateFlag(bool);
    void setCrosshairsFlag(bool);
    IndicatorPlot * getIndicatorPlot();
    DatePlot * getDatePlot();
    void setMenuFlag(bool);

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace(int);
    void setBackgroundColor(QColor);
    void setBorderColor(QColor);
    void setGridColor(QColor);
    void setPlotFont(QFont);
    void setIndex(int);
    void setInterval(BarData::BarLength);
    void markCrossCursor(int, int, bool);
    void markCrossHair(int, int, bool);
    void setChartPath(QString);
    void setCrosshairsStatus(bool);
    void slotSliderChanged(int);
    void slotGridChanged(bool);
    void slotScaleToScreenChanged(bool);
    void slotDrawModeChanged(bool);
    void slotDateFlagChanged(bool);
    void slotLogScaleChanged(bool);
    void slotUpdateScalePlot();

  private:
    DatePlot *datePlot;
    ScalePlot *scalePlot;
    IndicatorPlot *indicatorPlot;
};

#endif
