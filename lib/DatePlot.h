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

#ifndef DATEPLOT_HPP
#define DATEPLOT_HPP

#include <qwidget.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qfont.h>
#include <QVector>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <Q3PtrList>
#include "BarData.h"

typedef struct
{
  QString text;
  bool flag; // false == ignore
  bool tick; // true == long tick

} TickItem;

class DatePlot : public QWidget
{
  Q_OBJECT

  public:
    DatePlot(QWidget *);
    ~DatePlot();
    void clear();
    void setData(BarData *);
    QVector<int> & getXGrid();

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace(int);
    void setBackgroundColor(QColor);
    void setBorderColor(QColor);
    void setPlotFont(QFont);
    void setIndex(int);
    void setInterval(BarData::BarLength);

  protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

  private slots:
    void getDailyDate();
    void getWeeklyDate();
    void getMonthlyDate();
    void getMinuteDate();

  private:
    QFont plotFont;
    QPixmap buffer;
    int pixelspace;
    int scaleWidth;
    int startX;
    int startIndex;
    BarData::BarLength interval;
    QColor backgroundColor;
    QColor borderColor;
    BarData *data;
    Q3PtrList<TickItem> dateList;
    QVector<int> xGrid;
};

#endif
