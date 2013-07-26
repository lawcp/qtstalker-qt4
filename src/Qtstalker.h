/*
 *  Qtstalker - Commodity/Stock Charting and Technical Analysis Platform
 *  begin - Thu Mar  7 22:43:41 EST 2002
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

#ifndef QTSTALKER_H
#define QTSTALKER_H

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qsplitter.h>
#include <q3multilineedit.h>
#include <q3dict.h>
#include <q3progressbar.h>
#include <qstatusbar.h>
#include <qtabwidget.h>
//Added by qt3to4:
#include <Q3PtrList>

#include "Indicator.h"
#include "Plot.h"
#include "Config.h"
#include "Navigator.h"
#include "Setting.h"
#include "ChartPage.h"
#include "NavigatorTab.h"
#include "IndicatorPage.h"
#include "ScannerPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"
#include "GroupPage.h"
#include "ChartToolbar.h"
#include "MainMenubar.h"
#include "ExtraToolbar.h"
#include "DBIndex.h"
#include "RcFile.h"

// not used #define DEFAULT_INDICATOR_HEIGHT 125

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

  signals:
    void signalPixelspace(int);
    void signalBackgroundColor(QColor);
    void signalBorderColor(QColor);
    void signalGridColor(QColor);
    void signalPlotFont(QFont);
    void signalIndex(int);
    void signalInterval(BarData::BarLength);
    void signalChartPath(QString);
    void signalCrosshairsStatus(bool);

  public:

    enum chartStatus
    {
      None,
      Chart
    };

    QtstalkerApp();
    ~QtstalkerApp();
    void initConfig();
    void initMenuBar();
    void initToolBar();
    void initGroupNav();
    void initChartNav();
    void initPortfolioNav();
    void initTestNav();
    void initIndicatorNav();
    void initScannerNav();
    QString getWindowCaption();
    void loadChart(QString &);
    void barLengthChanged();
    void exportChart(QString &);
    void traverse(QString &);
    void loadIndicator(Indicator *);
    void setSliderStart();

  public slots:
    void slotAbout();
    void slotQuit();
    void slotOpenChart(QString);
    void slotQuotes();
    void slotOptions();
    void slotDataWindow();
    void slotNewIndicator(Indicator *);
    void slotEditIndicator(Indicator *);
    void slotDeleteIndicator(QString);
    void slotBarLengthChanged(int);
    void slotPixelspaceChanged(int);
    void slotChartUpdated();
    void slotStatusMessage(QString);
    void slotHideNav(bool);
    void slotUpdateInfo(Setting *);
    void slotPlotLeftMouseButton(int, int, bool);
    void slotPlotMouseMoved(int, int);
    void slotCrosshairsStatus(bool);
    void slotNavigatorPosition(int);
    void slotHelp();
    void slotDisableIndicator(QString);
    void slotEnableIndicator(QString);
    void slotProgMessage(int, int);
    void slotDrawPlots();
    void slotPaperTradeChanged(bool);
    void addIndicatorButton(QString);
    void slotWakeup();
    void slotIndicatorSummary();
    void slotDeleteAllCO();
    void slotDeleteCO(QString);
    void slotSaveCO(Setting);
    void slotMenubarStatus(bool);
    void slotExtraToolbarStatus(bool);
    void slotAppFont(QFont);
    void slotLoadMainToolbarSettings();
    void slotSavePlotSizes();
    void slotLoadPlotSizes();

  private:
    QToolBar *mainToolbar;
    ChartToolbar *chartToolbar;
    ExtraToolbar *extraToolbar;
    MainMenubar *menubar;
    QSplitter *split;
    QSplitter *navSplitter;
    QSplitter *dpSplitter;
    NavigatorTab *navTab;
    QWidget *baseWidget;
    QWidget *navBase;
    ChartPage *chartNav;
    Q3Dict<Plot> plotList;
    Config config;
    chartStatus status;
    QString chartPath;
    QString chartName;
    QString chartSymbol;
    QString dbPlugin;
    BarData *recordList;
    Q3MultiLineEdit *infoLabel;
    IndicatorPage *ip;
    PortfolioPage *pp;
    ScannerPage *sp;
    TestPage *tp;
    GroupPage *gp;
    Q3ProgressBar *progBar;
    QStatusBar *statusbar;
    QString chartType;
    Q3PtrList<QTabWidget> tabList;
    Q3Dict<QWidget> widgetList;
    DBIndex *chartIndex;
    RcFile rcfile;
    QString lastIndicatorUsed1;
    QString lastIndicatorUsed2;
    QString lastIndicatorUsed3;
};

#endif
