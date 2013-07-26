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

#include <qlayout.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <q3filedialog.h>
#include <qinputdialog.h>
#include <qapplication.h>
#include <qfont.h>
#include <QLocale>
#include <qtranslator.h>
#include <qcursor.h>
#include <qcolor.h>
#include <qstringlist.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include <math.h> // only for fabs()
#include <db.h>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "HelpWindow.h"
#include "DbPlugin.h"
#include "IndicatorSummary.h"
#include "Preferences.h"
#include "RcFile.h"

#include "../lib/qtstalker_defines.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp()
{
  recordList = 0;
  status = None;
  plotList.setAutoDelete(FALSE);
  tabList.setAutoDelete(TRUE);
  widgetList.setAutoDelete(TRUE);
  setIcon(qtstalker);

  // setup the disk environment
  config.setup();

  // open and init the chart index
  QString s;
  chartIndex = new DBIndex;

  //RcFile rcfile;
  rcfile.loadData(RcFile::IndexPath, s);
  chartIndex->open(s);

  initMenuBar();
  initToolBar();

  statusbar = statusBar();
  baseWidget = new QWidget(this);
  setCentralWidget(baseWidget);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(baseWidget);
  navSplitter = new QSplitter(baseWidget);
  navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(navSplitter);
  navBase = new QWidget(navSplitter);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(navBase);

  // setup the data panel splitter
  dpSplitter = new QSplitter(navBase);
  dpSplitter->setOrientation(Qt::Vertical);
  vbox->addWidget(dpSplitter);

  // setup the side panels
  navTab = new NavigatorTab(dpSplitter, this);
  connect(navTab, SIGNAL(signalPositionChanged(int)), this, SLOT(slotNavigatorPosition(int)));
  connect(navTab, SIGNAL(signaVisibilityChanged(bool)), this, SLOT(slotHideNav(bool)));
  connect(extraToolbar, SIGNAL(recentTab(QString)), navTab, SLOT(recentTab(QString)));

  // setup the data panel area
  infoLabel = new Q3MultiLineEdit(dpSplitter);
  infoLabel->setReadOnly(TRUE);

  // construct the chart areas
  QWidget *chartBase = new QWidget(navSplitter);

  vbox = new Q3VBoxLayout(chartBase);

  split = new QSplitter(chartBase);
  split->setOrientation(Qt::Vertical);
  vbox->addWidget(split);

  // build the tab rows
  int loop;
  for (loop = 0; loop < 3; loop++)
  {
    QTabWidget *it = new QTabWidget(split);
 //FIXME: Use setMargin(), otherwise uses default value 10 on some systems
 // use setContentsMargins() with Qt4
 //   it->setMargin(2);
    connect(it, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotDrawPlots()));
    tabList.append(it);
    it->hide();
  }

  // create the side panels
  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initPortfolioNav();
  initTestNav();
  initScannerNav();
  // aktivate last settings
  navTab->init();

  // restore the last used indicators
  rcfile.loadData(RcFile::LastIndicatorUsed, lastIndicatorUsed1, 1);
  rcfile.loadData(RcFile::LastIndicatorUsed, lastIndicatorUsed2, 2);
  rcfile.loadData(RcFile::LastIndicatorUsed, lastIndicatorUsed3, 3);

  // setup the initial indicators
  QString igroup;
  rcfile.loadData(RcFile::IndicatorGroup, igroup);
  QStringList l;
  config.getIndicators(igroup, l);
  for (loop = 0; loop < (int) l.count(); loop++)
    addIndicatorButton(l[loop]);

  // set the app font
  QFont font;
  rcfile.loadFont(RcFile::AppFont, font);
  slotAppFont(font);

  // place navigator on the last saved position
  navTab->loadSettings();
  navTab->togglePosition(navTab->getPosition());

  // restore the splitter sizes
  rcfile.loadSplitterSize(RcFile::NavAreaSize, navSplitter);
  slotLoadPlotSizes();
  rcfile.loadSplitterSize(RcFile::DataPanelSize,dpSplitter);

  // restore the size of the app
  QSize sz;
  rcfile.loadSize(RcFile::MainWindowSize, sz);
  resize(sz);

  // restore the position of the app
  //QPoint p;
  //rcfile.loadPoint(RcFile::MainWindowPos, p);
  //move(p);

  // setup the indicator page
  ip->updateList();

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));

  progBar = new Q3ProgressBar(this);
  statusbar->addWidget(progBar, 0, TRUE);
  progBar->setMaximumHeight(progBar->height() - 10);

  statusbar->message(tr("Ready"), 2000);
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initMenuBar()
{
  // create the main menubar
  menubar = new MainMenubar(this);
  //attach it...mainwindow takes ownership
  setMenuBar(menubar);
  connect(menubar, SIGNAL(signalExit()), qApp, SLOT(quit()));
  connect(menubar, SIGNAL(signalSidePanel(bool)), this, SLOT(slotHideNav(bool)));
  connect(menubar, SIGNAL(signalOptions()), this, SLOT(slotOptions()));
  connect(menubar, SIGNAL(signalQuotes()), this, SLOT(slotQuotes()));
  connect(menubar, SIGNAL(signalCrosshairs(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  connect(menubar, SIGNAL(signalPaperTrade(bool)), this, SLOT(slotPaperTradeChanged(bool)));
}

void QtstalkerApp::initToolBar()
{
  // construct the button toolbar
  mainToolbar = new QToolBar("buttonToolbar",this);
  //attach to qmainwindwo...
  addToolBar(Qt::TopToolBarArea,mainToolbar);
  mainToolbar->setLabel("Main Toolbar");
  slotLoadMainToolbarSettings();

  // construct the extra toolbar
  extraToolbar = new ExtraToolbar(this);
  //attach to qmainwindwo...
  addToolBar(Qt::TopToolBarArea,extraToolbar);
  extraToolbar->setLabel("Extra Toolbar");
  extraToolbar->slotSetButtonView();
  connect(extraToolbar, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  addToolBarBreak();

  // construct the chart toolbar
  chartToolbar = new ChartToolbar(this);
  //attach to qmainwindwo...
  addToolBar(Qt::TopToolBarArea,chartToolbar);
  chartToolbar->setLabel("Chart Toolbar");
  chartToolbar->slotSetButtonView();
  connect(chartToolbar, SIGNAL(signalBarLengthChanged(int)), this, SLOT(slotBarLengthChanged(int)));
  connect(chartToolbar, SIGNAL(signalPixelspaceChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  connect(chartToolbar, SIGNAL(signalBarsChanged(int)), this, SLOT(slotChartUpdated()));
  connect(chartToolbar, SIGNAL(signalPaperTradeNextBar()), this, SLOT(slotChartUpdated()));
  chartToolbar->paperTradeClicked(menubar->getStatus(MainMenubar::PaperTrade));
  connect(menubar, SIGNAL(signalAdvancePaperTrade()), chartToolbar, SLOT(paperTradeNextBar()));
}

void QtstalkerApp::slotLoadMainToolbarSettings()
{
  mainToolbar->clear();
  bool tb;

  rcfile.loadData(RcFile::ShowQuitBtn, tb);
  if(tb) menubar->getAction(MainMenubar::Exit)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowPrefBtn, tb);
  if(tb) menubar->getAction(MainMenubar::Options)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowSidePanelBtn, tb);
  if(tb) menubar->getAction(MainMenubar::SidePanel)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowGridBtn, tb);
  if(tb) menubar->getAction(MainMenubar::Grid)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowScaleToScreenBtn, tb);
  if(tb) menubar->getAction(MainMenubar::ScaleToScreen)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowCrosshairBtn, tb);
  if(tb) menubar->getAction(MainMenubar::Crosshairs)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowDrawModeBtn, tb);
  if(tb) menubar->getAction(MainMenubar::DrawMode)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowPaperTradeBtn, tb);
  if(tb) menubar->getAction(MainMenubar::PaperTrade)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowNewIndicatorBtn, tb);
  if(tb) menubar->getAction(MainMenubar::NewIndicator)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowDataWindowBtn, tb);
  if(tb) menubar->getAction(MainMenubar::DataWindow)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowMainQuoteBtn, tb);
  if(tb) menubar->getAction(MainMenubar::Quotes)->addTo(mainToolbar);
  rcfile.loadData(RcFile::ShowHelpButton, tb);
  if(tb) menubar->getAction(MainMenubar::Help)->addTo(mainToolbar);
}

void QtstalkerApp::slotQuit()
{
  // do this to save any pending chart object edits
  Q3DictIterator<Plot> it(plotList);
  for (; it.current(); ++it)
    it.current()->clear();

  // save last indicators used
  int loop;
  for (loop = 0; loop < (int)tabList.count(); loop++)
  {
    QTabWidget *tw = tabList.at(loop);
    QString s = tw->label(tw->currentPageIndex());
    rcfile.saveData(RcFile::LastIndicatorUsed, s, loop+1);
  }

  // save window sizes
  //RcFile rcfile;
  slotSavePlotSizes();
  rcfile.saveSplitterSize(RcFile::DataPanelSize, dpSplitter);
  rcfile.saveSplitterSize(RcFile::NavAreaSize, navSplitter);

  rcfile.saveSize(RcFile::MainWindowSize, size());
  //rcfile.savePoint(RcFile::MainWindowPos, pos());

  config.closePlugins();

  // make sure we clean up the local indicators before we quit
  ip->removeLocalIndicators();

  menubar->saveSettings();
  chartToolbar->saveSettings();
  extraToolbar->saveSettings();

  //no delete needed anymore, qmainwindow is owner
  //delete menubar;
  delete extraToolbar;
  delete chartToolbar;

  // delete any BarData
  if (recordList)
    delete recordList;

  chartIndex->close();

  // call the destructors which save some settings
  delete gp;
  delete chartNav;
  delete navTab;
}

void QtstalkerApp::slotAbout()
{
  // display the about dialog
  int major, minor, patch;
  QString versionString = "Qtstalker\nVersion ";
  versionString += PG_VERSION_STRING;
  versionString += " (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += ", ";
  versionString += DB_VERSION_STRING;
  versionString += "\nRunning Qt ";
  versionString += qVersion();
  versionString += ", ";
  versionString += db_version(&major, &minor, &patch);
  versionString += "\n\nIRC: irc.freenode.net - #qtstalker, #qtstalker-in";
  versionString += "\n\n(C) 2001-2010 by Stefan Stratigakos";
  versionString += "\n(C) 2007-2010 by Balwinder S Dheeman";
  versionString += "\nhttp://qtstalker.sourceforge.net/";
  versionString += "\nLicense: GNU General Public License, Version 2";
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  					versionString,
					QMessageBox::NoIcon,
					QMessageBox::Ok,
					QMessageBox::NoButton,
					QMessageBox::NoButton,
					this,
					"AboutDialog",
					FALSE);
  dialog->setIconPixmap(qtstalker);
  dialog->show();
}

void QtstalkerApp::slotOpenChart(QString selection)
{
  // load a chart slot
  chartToolbar->enableSlider(TRUE);
  status = Chart;
  qApp->processEvents();
  loadChart(selection);
}

void QtstalkerApp::slotQuotes()
{
  //RcFile rcfile;
  QStringList l;
  config.getPluginList(Config::QuotePluginPath, l);
  QString s;
  rcfile.loadData(RcFile::LastQuotePlugin, s);
  int i = l.findIndex(s);
  if (i == -1)
    i = 0;

  bool ok;
  s = QInputDialog::getItem(tr("Quote Dialog"), tr("Select Quote Type"), l, i, FALSE, &ok, this, 0);
  if (! ok)
    return;

  QuotePlugin *plug = config.getQuotePlugin(s);
  if (! plug)
    return;

  connect(plug, SIGNAL(chartUpdated()), this, SLOT(slotChartUpdated()));
  connect(plug, SIGNAL(signalWakeup()), this, SLOT(slotWakeup()));

  plug->setChartIndex(chartIndex);

  rcfile.saveData(RcFile::LastQuotePlugin, s);

  plug->show();
}

void QtstalkerApp::slotOptions()
{
  Preferences *dialog = new Preferences(this);
  connect(dialog, SIGNAL(signalMenubar(bool)), this, SLOT(slotMenubarStatus(bool)));
  connect(dialog, SIGNAL(signalExtraToolbar(bool)), this, SLOT(slotExtraToolbarStatus(bool)));
  connect(dialog, SIGNAL(signalBackgroundColor(QColor)), this, SIGNAL(signalBackgroundColor(QColor)));
  connect(dialog, SIGNAL(signalBorderColor(QColor)), this, SIGNAL(signalBorderColor(QColor)));
  connect(dialog, SIGNAL(signalGridColor(QColor)), this, SIGNAL(signalGridColor(QColor)));
  connect(dialog, SIGNAL(signalPlotFont(QFont)), this, SIGNAL(signalPlotFont(QFont)));
  connect(dialog, SIGNAL(signalAppFont(QFont)), this, SLOT(slotAppFont(QFont)));
  connect(dialog, SIGNAL(signalLoadChart()), this, SLOT(slotChartUpdated()));
  connect(dialog, SIGNAL(signalReloadToolBars()), this, SLOT(slotLoadMainToolbarSettings()));
  connect(dialog, SIGNAL(signalReloadToolBars()), chartToolbar, SLOT(slotSetButtonView()));
  connect(dialog, SIGNAL(signalReloadToolBars()), extraToolbar, SLOT(slotSetButtonView()));

  dialog->show();
}

void QtstalkerApp::loadChart(QString &d)
{
  // do all the stuff we need to do to load a chart
  if (d.length() == 0)
    return;

  QDir dir(d);
  if (! dir.exists(d))
    return;

  // check if we need to save the slider position because chart is reloaded
  bool reload = FALSE;
  if (! chartPath.compare(d))
    reload = TRUE;

  chartPath = d;
  ip->setChartPath(chartPath); // let ip know what chart we are viewing currently

  Q3DictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();

  // make sure we change db's after Plot saves previous db edits
  emit signalChartPath(chartPath);

  // open the chart
  QFileInfo fi(chartPath);
  QString fn = fi.fileName();
  DbPlugin db;

  if (db.open(chartPath, chartIndex))
  {
    qDebug("QtstalkerApp::loadChart: can't open %s", chartPath.latin1());
    return;
  }

  rcfile.saveData(RcFile::CurrentChart, chartPath);

  DBIndexItem item;
  chartIndex->getIndexItem(fn, item);
  item.getTitle(chartName);
  item.getType(chartType);
  item.getSymbol(chartSymbol);

//  connect(db, SIGNAL(signalMessage(int, int)), this, SLOT(slotProgMessage(int, int)));
//  connect(db, SIGNAL(signalMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  db.setBarLength((BarData::BarLength) chartToolbar->getBarLengthInt());
  db.setBarRange(chartToolbar->getBars());

  if (recordList)
    delete recordList;
  recordList = new BarData(chartPath);
  QString s;
  recordList->setBarType((BarData::BarType) item.getBarType());
  recordList->setBarLength((BarData::BarLength) chartToolbar->getBarLengthInt());

  slotStatusMessage(tr("Loading chart..."));

  QDateTime dt = QDateTime::currentDateTime();
  if (menubar->getStatus(MainMenubar::PaperTrade))
    chartToolbar->getPaperTradeDate(dt);
  db.getHistory(recordList, dt);

  slotProgMessage(1, 3);
  slotStatusMessage(tr("Loading indicators..."));

  for(it.toFirst(); it.current(); ++it)
    it.current()->setData(recordList);

  // setup the local indicators
  if (! reload)
  {
    ip->removeLocalIndicators();
    chartIndex->getIndicators(chartSymbol, s);
    ip->addLocalIndicators(s);
  }
  QStringList l;
  s = ip->getIndicatorGroup();
  config.getIndicators(s, l);

  int loop;
  for (loop = 0; loop < (int)l.count(); loop++)
  {
    Setting set;
    config.getIndicator(l[loop], set);
    if (! set.count())
      continue;

    Indicator *i = new Indicator;
    i->setIndicator(set, l[loop]);
    if (! i->getEnable())
    {
      delete i;
      continue;
    }

    loadIndicator(i);
  }

  slotProgMessage(2, 3);
  slotStatusMessage(tr("Loading chart objects..."));

  chartIndex->getChartObjects(fn, l);
  Setting co;
  for (loop = 0; loop < (int)l.count(); loop++)
  {
    co.parse(l[loop]);
    s = "Plot";
    QString s2;
    co.getData(s, s2);
    Plot *plot = plotList[s2];
    if (plot)
      plot->addChartObject(co);
    else
      qDebug("QtstalkerApp::loadChart: plot %s not found for loading chart object", s2.latin1());
  }

  db.close();
  setSliderStart();
  slotDrawPlots();
  ip->updateList();
  setCaption(getWindowCaption());
  slotProgMessage(-1, 3);
  slotStatusMessage(QString());
}

void QtstalkerApp::loadIndicator(Indicator *i)
{
  // create and prep an indicator for display

  QString plugin;
  i->getType(plugin);
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
    return;

  connect(plug, SIGNAL(signalWakeup()), this, SLOT(slotWakeup()));
  if (recordList)
    plug->setIndicatorInput(recordList);
  QString s;
  i->getFile(s);
  plug->loadIndicatorSettings(s);
  Indicator *ti = 0;
  if (recordList)
    ti = plug->calculate();
  if (ti)
    i->copy(ti);
  if (ti)
    delete ti;

  QFileInfo fi(s);
  Plot *plot = plotList[fi.fileName()];
  if (! plot)
  {
    qDebug("QtstalkerApp::loadIndicator:plot %s not found", fi.fileName().latin1());
    return;
  }
  plot->addIndicator(i);
  if (recordList)
    plot->setData(recordList);
}

QString QtstalkerApp::getWindowCaption()
{
  // update the main window text

  QString caption = tr("Qtstalker");

  switch(status)
  {
    case Chart:
      caption.append(": ");
      caption.append(chartName);
      caption.append("(");
      if (chartSymbol.length() > 0)
        caption.append(chartSymbol);
      else
        caption.append(chartType);
      caption.append(") ");
      break;
    default:
      break;
  }

  caption.append(chartToolbar->getBarLength());

  return caption;
}

void QtstalkerApp::slotDataWindow()
{
  // show the datawindow dialog

  if (! recordList)
    return;

  DataWindow *dw = new DataWindow(0);
  dw->setCaption(getWindowCaption());

  dw->setBars(recordList);

  Q3DictIterator<Plot> it(plotList);
  for (; it.current(); ++it)
    dw->setPlot(it.current());

  dw->show();
}

void QtstalkerApp::slotBarLengthChanged(int barLength)
{
  QString s;
  rcfile.loadData(RcFile::BarLength, s);
  int n = s.toInt();
  if (n == barLength) return;

  // the compression has changed
  barLengthChanged();
  loadChart(chartPath);
}

void QtstalkerApp::barLengthChanged()
{
  // bar length has changed
  QString s = QString::number(chartToolbar->getBarLengthInt());
  rcfile.saveData(RcFile::BarLength, s);

  emit signalInterval((BarData::BarLength) chartToolbar->getBarLengthInt());
}

void QtstalkerApp::slotNewIndicator(Indicator *i)
{
  // add a new indicator slot
  QString str;
  i->getFile(str);
  addIndicatorButton(str);
  loadIndicator(i);
  ip->updateList();
}

void QtstalkerApp::slotEditIndicator(Indicator *i)
{
  // edit indicator slot

  loadIndicator(i);

  QString s;
  i->getFile(s);
  QFileInfo fi(s);
  Plot *p = plotList[fi.fileName()];
  if (p)
    p->draw();
}

void QtstalkerApp::slotDeleteIndicator(QString text)
{
  // delete indicator slot

  QString s;
  //RcFile rcfile;
  rcfile.loadData(RcFile::IndicatorPath, s);
  s.append("/" + ip->getIndicatorGroup() + "/" + text);
  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::slotDeleteIndicator:indicator settings empty");
    return;
  }

  // delete any chart objects that belong to the indicator
  Indicator i;
  i.setIndicator(set, s);
  QTabWidget *it = tabList.at(i.getTabRow() - 1);
  it->removePage(widgetList[text]);
  widgetList.remove(text);

  if (! it->count())
    it->hide();

  plotList.remove(text);

  config.deleteIndicator(s);

  ip->updateList();
}

void QtstalkerApp::slotDisableIndicator(QString name)
{
  // remove indicator
  Setting set;
  config.getIndicator(name, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::slotDisableIndicator:indicator settings empty");
    return;
  }

  Indicator i;
  i.setIndicator(set, name);
  QTabWidget *it = tabList.at(i.getTabRow() - 1);
  QString s;
  i.getName(s);
  it->removePage(widgetList[s]);
  widgetList.remove(s);

  if (! it->count())
    it->hide();

  plotList.remove(s);
}

void QtstalkerApp::slotEnableIndicator(QString name)
{
  // add indicator
  Setting set;
  config.getIndicator(name, set);
  if (! set.count())
    return;

  Indicator *i = new Indicator;
  i->setIndicator(set, name);
  if (i->getEnable() == 0)
  {
    delete i;
    return;
  }

  addIndicatorButton(name);
  loadIndicator(i);
}

void QtstalkerApp::slotPixelspaceChanged(int d)
{
  emit signalPixelspace(d);
  emit signalIndex(chartToolbar->getSlider());
  setSliderStart();
  slotDrawPlots();
}

void QtstalkerApp::addIndicatorButton(QString d)
{
  Setting set;
  config.getIndicator(d, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::addIndicatorButton:indicator settings empty");
    return;
  }

  Indicator i;
  i.setIndicator(set, d);
  if (i.getEnable() == 0)
    return;

  QString type;
  i.getType(type);
  int row = i.getTabRow();

  QFileInfo fi(d);
  QString fn = fi.fileName();

  QWidget *w = new QWidget(baseWidget);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(0);
  vbox->setSpacing(0);
  Plot *plot = new Plot(w, chartIndex);
  vbox->addWidget(plot);
  widgetList.replace(fi.fileName(), w);

  plotList.replace(fi.fileName(), plot);

  QTabWidget *it = tabList.at(row - 1);
  it->addTab(w, fi.fileName());

  if (it->isHidden())
    it->show();

  // Set the current indicator in this row to the last used one.
  switch (row)
  {
    case 1:
      if (fn == lastIndicatorUsed1)
        it->setCurrentPage(it->indexOf(w));
      break;
    case 2:
      if (fn == lastIndicatorUsed2)
        it->setCurrentPage(it->indexOf(w));
      break;
    case 3:
      if (fn == lastIndicatorUsed3)
        it->setCurrentPage(it->indexOf(w));
      break;
    default:
      break;
  }

  QColor color;
  rcfile.loadColor(RcFile::BackgroundColor, color);

  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  plot->setBackgroundColor(color);

  rcfile.loadColor(RcFile::BorderColor, color);
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  plot->setBorderColor(color);

  rcfile.loadColor(RcFile::GridColor, color);
  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  plot->setGridColor(color);

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  QFont font;
  rcfile.loadFont(RcFile::PlotFont, font);
  plot->setPlotFont(font);

  plot->setGridFlag(menubar->getStatus(MainMenubar::Grid));
  plot->setScaleToScreen(menubar->getStatus(MainMenubar::ScaleToScreen));
  plot->setPixelspace(chartToolbar->getPixelspace());
  plot->setIndex(chartToolbar->getSlider());
  plot->setInterval((BarData::BarLength) chartToolbar->getBarLengthInt());
  bool b;
  rcfile.loadData(RcFile::Crosshairs, b);
  plot->setCrosshairsStatus(b);
  plot->setDrawMode(menubar->getStatus(MainMenubar::DrawMode));

  connect(plot->getIndicatorPlot(), SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(plot->getIndicatorPlot(), SIGNAL(signalEditIndicator(QString)), ip, SLOT(editIndicator(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  connect(plot->getIndicatorPlot(), SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  connect(plot->getIndicatorPlot(), SIGNAL(mouseMoved(int, int)), this, SLOT(slotPlotMouseMoved(int, int)));
  connect(plot->getIndicatorPlot(), SIGNAL(signalEditChart(QString)), chartNav, SLOT(editChart(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(signalDeleteAllCO()), this, SLOT(slotDeleteAllCO()));
  connect(plot->getIndicatorPlot(), SIGNAL(signalDeleteCO(QString)), this, SLOT(slotDeleteCO(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(signalSaveCO(Setting)), this, SLOT(slotSaveCO(Setting)));
  connect(this, SIGNAL(signalCrosshairsStatus(bool)), plot->getIndicatorPlot(), SLOT(setCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
  connect(this, SIGNAL(signalChartPath(QString)), plot, SLOT(setChartPath(QString)));

  connect(chartToolbar, SIGNAL(signalSliderChanged(int)), plot, SLOT(slotSliderChanged(int)));
  connect(menubar, SIGNAL(signalGrid(bool)), plot->getIndicatorPlot(), SLOT(slotGridChanged(bool)));
  connect(menubar, SIGNAL(signalScale(bool)), plot, SLOT(slotScaleToScreenChanged(bool)));
  connect(menubar, SIGNAL(signalDraw(bool)), plot->getIndicatorPlot(), SLOT(slotDrawModeChanged(bool)));
}

void QtstalkerApp::slotChartUpdated()
{
  chartNav->refreshList();

  if (status == None)
    return;

  int ti = chartToolbar->getBars();
  rcfile.saveData(RcFile::BarsToLoad, ti);

  loadChart(chartPath);
}

void QtstalkerApp::slotStatusMessage(QString d)
{
  statusbar->message(d);
  qApp->processEvents();
}

void QtstalkerApp::initGroupNav()
{
  gp = new GroupPage(baseWidget);
  connect(gp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  connect(chartNav, SIGNAL(signalAddToGroup(QString)), gp, SLOT(addChartToGroup(QString)));
  connect(gp, SIGNAL(addRecentChart(QString)), extraToolbar, SLOT(slotAddRecentChart(QString)));
  connect(gp, SIGNAL(removeRecentCharts(QStringList)), extraToolbar, SLOT(slotRemoveRecentCharts(QStringList)));
  connect(extraToolbar, SIGNAL(signalSetGroupNavItem(QString, QString)), gp, SLOT(setGroupNavItem(QString, QString)));
  navTab->addWidget(gp, 1);
}

void QtstalkerApp::initChartNav()
{
  chartNav = new ChartPage(baseWidget, chartIndex);
  connect(chartNav, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  connect(chartNav, SIGNAL(addRecentChart(QString)), extraToolbar, SLOT(slotAddRecentChart(QString)));
  connect(chartNav, SIGNAL(removeRecentCharts(QStringList)), extraToolbar, SLOT(slotRemoveRecentCharts(QStringList)));
  connect(chartNav, SIGNAL(signalReloadChart()), this, SLOT(slotChartUpdated()));
  connect(extraToolbar, SIGNAL(signalSetChartNavItem(QString, QString)), chartNav, SLOT(setChartNavItem(QString, QString)));
  navTab->addWidget(chartNav, 0);
}

void QtstalkerApp::initPortfolioNav()
{
  pp = new PortfolioPage(baseWidget, chartIndex);
  navTab->addWidget(pp, 3);
}

void QtstalkerApp::initTestNav()
{
  tp = new TestPage(baseWidget, menubar, chartIndex);
  connect(tp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addWidget(tp, 4);
}

void QtstalkerApp::initIndicatorNav()
{
  ip = new IndicatorPage(baseWidget, chartIndex);
  connect(ip, SIGNAL(signalDisableIndicator(QString)), this, SLOT(slotDisableIndicator(QString)));
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(slotEnableIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator(Indicator *)), this, SLOT(slotNewIndicator(Indicator *)));
  connect(ip, SIGNAL(signalEditIndicator(Indicator *)), this, SLOT(slotEditIndicator(Indicator *)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(slotDeleteIndicator(QString)));
  connect(menubar, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(ip, SIGNAL(signalGroupIsChanging()), this, SLOT(slotSavePlotSizes()));
  connect(ip, SIGNAL(signalGroupChanged()), this, SLOT(slotLoadPlotSizes()));
  connect(ip, SIGNAL(signalGroupChanged()), this, SLOT(slotChartUpdated()));
  connect(ip, SIGNAL(signalLocalIndicator(QString)), this, SLOT(addIndicatorButton(QString)));
  navTab->addWidget(ip, 2);
}

void QtstalkerApp::initScannerNav()
{
  sp = new ScannerPage(baseWidget, chartIndex);
  connect(sp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(sp, SIGNAL(refreshGroup()), gp, SLOT(refreshList()));
  navTab->addWidget(sp, 5);
}

void QtstalkerApp::slotHideNav(bool d)
{
  menubar->setStatus(MainMenubar::SidePanel, d);

  if (d)
    navBase->show();
  else
    navBase->hide();
}

void QtstalkerApp::slotUpdateInfo(Setting *r)
{
  // list bar values first
  QStringList l;
  l.append("D");
  l.append("T");
  l.append("O");
  l.append("H");
  l.append("L");
  l.append("C");
  l.append("VOL");
  l.append("OI");
  QString s, s2, str;
  int loop;
  for (loop = 0; loop < (int)l.count(); loop++)
  {
    r->getData(l[loop], s);
    if (s.length())
    {
      str.append(l[loop] + " " + s + "\n");
      r->remove(l[loop]);
    }
  }

  r->getKeyList(l);
  l.sort();
  for (loop = 0; loop < (int)l.count(); loop++)
  {
    r->getData(l[loop], s);
    // If it is a big number, then use zero precision.
    bool ok;
    double sn = s.toDouble(&ok);
    if (ok) {
      if (fabs(sn) > 1000)
        s = QString::number(sn, 'f', 0);
    }
    str.append(l[loop] + " " + s + "\n");
  }

  delete r;

  infoLabel->setText(str);
}

void QtstalkerApp::slotPlotLeftMouseButton(int x, int y, bool)
{
  Q3DictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->markCrossHair(x, y, FALSE);
  slotDrawPlots();
}

void QtstalkerApp::slotPlotMouseMoved(int x, int y)
{
  Q3DictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->markCrossCursor(x, y, FALSE);
  slotDrawPlots();
}

void QtstalkerApp::slotCrosshairsStatus(bool status)
{
  emit signalCrosshairsStatus(status);
}

void QtstalkerApp::setSliderStart()
{
  if (status == None)
    return;

  int loop;
  int rc = 0;
  for (loop = 0; loop < (int)tabList.count(); loop++)
  {
    QTabWidget *tw = tabList.at(loop);
    if (! tw->isHidden())
    {
      Plot *plot = plotList[tw->label(tw->currentPageIndex())];
      if (! plot)
        return;
      else
      {
        rc = chartToolbar->setSliderStart(plot->getWidth(), recordList->count());
        break;
      }
    }
  }

  emit signalIndex(rc);
}

void QtstalkerApp::slotNavigatorPosition(int d)
{
  if (! d)
    navSplitter->moveToFirst(navBase);
  else
    navSplitter->moveToLast(navBase);

  navSplitter->refresh();
}

void QtstalkerApp::slotHelp()
{
  QString s = "toc.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void QtstalkerApp::slotProgMessage(int p, int t)
{
  if (p == -1)
    progBar->reset();
  else
    progBar->setProgress(p, t);

  qApp->processEvents();
}

void QtstalkerApp::slotDrawPlots()
{
  int loop;
  for(loop = 0; loop < (int)tabList.count(); loop++)
  {
    QTabWidget *it = tabList.at(loop);
    if (it->isHidden())
      continue;
    QString s = it->label(it->currentPageIndex());
    Plot *p = plotList[s];
    if (p)
      p->draw();
  }
}

void QtstalkerApp::slotPaperTradeChanged(bool d)
{
  chartToolbar->paperTradeClicked(d);
  slotChartUpdated();
}

void QtstalkerApp::slotWakeup()
{
  qApp->processEvents();
}

void QtstalkerApp::slotIndicatorSummary()
{
  if (status == None)
    return;

  QString basePath, s;
  //RcFile rcfile;
  rcfile.loadData(RcFile::IndicatorPath, basePath);
  rcfile.loadData(RcFile::IndicatorGroup, s);
  basePath.append("/" + s);

  QStringList l;
  Q3DictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    Indicator *i = it.current()->getIndicator();
    i->getName(s);
    l.append(basePath + "/" + s);
  }

  IndicatorSummary is(l, chartToolbar->getBars(), (BarData::BarLength) chartToolbar->getBarLengthInt(), chartIndex);
  connect(&is, SIGNAL(signalWakeup()), this, SLOT(slotWakeup()));
  is.run();
}

void QtstalkerApp::slotDeleteAllCO()
{
  chartIndex->deleteAllChartObjects(chartSymbol);
}

void QtstalkerApp::slotDeleteCO(QString d)
{
  chartIndex->deleteChartObject(chartSymbol, d);
}

void QtstalkerApp::slotSaveCO(Setting d)
{
  QString s = "Name";
  QString s2;
  d.getData(s, s2);
  chartIndex->setChartObject(chartSymbol, s2, d);
}

void QtstalkerApp::slotMenubarStatus(bool d)
{
  if (d)
    menubar->show();
  else
    menubar->hide();
}

void QtstalkerApp::slotExtraToolbarStatus(bool d)
{
  if (d)
    extraToolbar->show();
  else
    extraToolbar->hide();
}

void QtstalkerApp::slotAppFont(QFont d)
{
  qApp->setFont(d, TRUE, 0);
  qApp->setFont(d, TRUE, "QComboBox");
}

void QtstalkerApp::slotSavePlotSizes()
{
  QString s;
  s = ip->getIndicatorGroup();
  rcfile.saveSplitterSize(RcFile::PlotSizes, split, s);
}

void QtstalkerApp::slotLoadPlotSizes()
{
  QString s;
  s = ip->getIndicatorGroup();
  rcfile.loadSplitterSize(RcFile::PlotSizes, split, s);
}

//**********************************************************************
//**********************************************************************
//**********************************************************************

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("QtStalker");
  QCoreApplication::setApplicationName("QtStalker");

  QTranslator translator;
  QString i18nDir = QString("%1/qtstalker/i18n").arg(INSTALL_DATA_DIR);
  QString i18nFile = QString("qtstalker_%1").arg(QLocale::system().name());
  translator.load(i18nFile, i18nDir);
  app.installTranslator(&translator);

  QtstalkerApp *qtstalker = new QtstalkerApp();
  qtstalker->setCaption("Qtstalker");
  qtstalker->show();
  app.setMainWidget(qtstalker);

  return app.exec();
}
