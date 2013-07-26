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

#ifndef SCALEPLOT_HPP
#define SCALEPLOT_HPP

#include <qwidget.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qfont.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include "Scaler.h"

class ScalePlot : public QWidget
{
  Q_OBJECT

  public:
    ScalePlot(QWidget *);
    ~ScalePlot();
    void clear();
    void setData(double);
    void setScaleWidth(int);
    void setLogScale(bool);
    void setScaleToScreen(bool);
    void strip(double, int, QString &);
    void setScaler(Scaler &);

  public slots:
    void draw();
    void drawRefresh();
    void setBackgroundColor(QColor &);
    void setBorderColor(QColor &);
    void setPlotFont(QFont &);
    void slotScaleToScreenChanged(bool);
    void slotLogScaleChanged(bool);
    void drawScale();
    void setMainFlag(bool);

  protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

  private:
    QFont plotFont;
    QPixmap buffer;
    QColor backgroundColor;
    QColor borderColor;
    bool scaleToScreen;
    bool logScale;
    int scaleWidth;
    double close;
    Scaler scaler;
    bool mainFlag;
    bool activeFlag;
};

#endif
