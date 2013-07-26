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

#ifndef INDICATORPLOT_HPP
#define INDICATORPLOT_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qcolor.h>
#include <QVector>
#include <qfont.h>
#include <q3dict.h>
#include <qstringlist.h>
#include <qmenu.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include "PlotLine.h"
#include "Setting.h"
#include "Indicator.h"
#include "Scaler.h"
#include "Config.h"
#include "BarData.h"
#include "COBase.h"
#include "DBIndex.h"

class IndicatorPlot : public QWidget
{
  Q_OBJECT

  signals:
    void statusMessage(QString);
    void infoMessage(Setting *);
    void leftMouseButton(int, int, bool);
    void mouseMoved(int, int);
    void keyPressed(QKeyEvent *);
    void signalEditIndicator(QString);
    void signalNewIndicator();
    void signalMinPixelspace(int);
    void signalDraw();
    void signalDateFlag(bool);
    void signalLogFlag(bool);
    void signalEditChart(QString);
    void signalDeleteAllCO();
    void signalDeleteCO(QString);
    void signalSaveCO(Setting);

  public:
    enum MouseStatus
    {
      None,
      ClickWait,
      COSelected,
      Moving
    };

    IndicatorPlot(QWidget *, DBIndex *);
    ~IndicatorPlot();
    void clear();
    void setData(BarData *);
    void setLogScale(bool);
    void updateStatusBar(int, int);
    void setInfoFlag(bool);
    void drawCrossCursor();
    void drawCrossHair();
    void addIndicator(Indicator *);
    Indicator * getIndicator();
    bool deleteIndicator();
    void addChartObject(Setting &);
    int getWidth();
    void strip(double, int, QString &);
    int convertXToDataIndex(int);
    void setGridFlag(bool);
    void setScaleToScreen(bool);
    void setDrawMode(bool);
    void setDateFlag(bool);
    void setCrosshairsFlag(bool);
    void setScaler(Scaler &);
    Scaler & getScaler();
    void getInfo(int);
    void setXGrid(QVector<int> &);
    void setMenuFlag(bool);

    void drawLine();
    void drawHorizontalLine();
    void drawHistogram();
    void drawHistogramBar();
    void drawDot();
    void drawBar();
    void drawCandle();
    void drawPF();

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace(int);
    void setBackgroundColor(QColor &);
    void setBorderColor(QColor &);
    void setGridColor(QColor &);
    void setPlotFont(QFont &);
    void setIndex(int);
    void markCrossCursor(int, int, bool);
    void markCrossHair(int, int, bool);
    void printChart();
    void showPopupMenu();
    void setChartPath(QString &);
    void setCrosshairsStatus(bool);
    void slotSliderChanged(int);
    void slotGridChanged(bool);
    void slotScaleToScreenChanged(bool);
    void slotDrawModeChanged(bool);
    void slotLogScaleChanged(bool);
    void setInterval(BarData::BarLength);
    void slotEditChart();
    void slotIndicatorHelp();

  protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *);

  private slots:
    void drawObjects();
    void drawLines();
    void drawXGrid();
    void drawYGrid();
    void drawInfo();
    void setScale();
    int getXFromDate(QDateTime &);
    void getXY(int, int);
    void slotMessage(QString);
    void slotEditIndicator();
    void slotNewIndicator();
    void slotNewChartObject(int);
    void slotDeleteAllChartObjects();
    void slotChartObjectDeleted(QString);
    void toggleDate();
    void toggleLog();
    void saveChartObjects();
    void saveChartObject(QString);

  private:
    BarData *data;
    BarData::BarLength interval;
    COBase *coSelected;
    Config config;
    DBIndex *chartIndex;
    Indicator *indy;
    MouseStatus mouseFlag;
    PlotLine *currentLine;
    Q3Dict<COBase> coList;
    QColor backgroundColor;
    QColor borderColor;
    QColor gridColor;
    QDateTime crossHairX;
    QDateTime x1;
    QFont plotFont;
    QFontMetrics *plotFontMetrics;
    QMenu *chartMenu;
    QMenu *chartObjectMenu;
    QPixmap buffer;
    QString chartPath;
    QString docsPath;
    QString userDocsPath;
    QVector<int> xGrid;
    Scaler scaler;
    bool crossHairFlag;
    bool crosshairs;
    bool dateFlag;
    bool drawMode;
    bool gridFlag;
    bool infoFlag;
    bool logScale;
    bool menuFlag;
    bool scaleToScreen;
    double crossHairY;
    double y1;
    int pixelspace;
    int startIndex;
    int startX;
};

#endif
