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

#include <qstringlist.h>
#include <qsettings.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "RcFile.h"
#include "qtstalker_defines.h"

// This class do ALL but NOTHING ELSE with the qtstalkerrc file
// located in /home/yourhome/.qt/ or similar, depending on your system
//
// It is intended to replace the according functions
// locatet in the Config class. Because Config do more than
// only read/write stettings to and from the rc-file.
// But Config is often used/included to do only this.
// loh.tar


RcFile::RcFile()
{
  Key[Home] = "Home";
  Def[Home] = "";
  Key[DataPath] = "DataPath";
  Def[DataPath] = "";
  Key[Pixelspace] = "Pixelspace";
  Def[Pixelspace] = "6";
  Key[BarLength] = "BarLength";
  Def[BarLength] = "6"; // daily
  Key[Grid] = "Grid";
  Def[Grid] = "TRUE";
  Key[BarsToLoad] = "BarsToLoad";
  Def[BarsToLoad] = "475";
  Key[BackgroundColor] = "BackgroundColor";
  Def[BackgroundColor] = "#000000"; // black
  Key[BorderColor] = "BorderColor";
  Def[BorderColor] = "#ffffff"; // white
  Key[GridColor] = "GridColor";
  Def[GridColor] = "#555555"; // grey
  Key[PaperTradeMode] = "PaperTradeMode";
  Def[PaperTradeMode] = "FALSE";
  Key[IndicatorPath] = "IndicatorPath";
  Def[IndicatorPath] = "";
  Key[Crosshairs] = "Crosshairs";
  Def[Crosshairs] = "TRUE";
  Key[DrawMode] = "DrawMode";
  Def[DrawMode] = "FALSE";
  Key[DataPanelSize] = "DataPanelSize";
  Def[DataPanelSize] = "315,174";
  Key[ScaleToScreen] = "ScaleToScreen";
  Def[ScaleToScreen] = "TRUE";
  Key[QuotePluginPath] = "QuotePluginPath";
  Def[QuotePluginPath] = "";
  Key[GroupPath] = "GroupPath";
  Def[GroupPath] = "";
  Key[PortfolioPath] = "PortfolioPath";
  Def[PortfolioPath] = "";
  Key[TestPath] = "TestPath";
  Def[TestPath] = "";
  Key[PlotFont] = "PlotFont";
  Def[PlotFont] = "DejaVu Sans,10,50,0,0";
  Key[AppFont] = "AppFont";
  Def[AppFont] = "DejaVu Sans,10,50,0,0";
  Key[NavAreaSize] = "NavAreaSize";
  Def[NavAreaSize] = "671,115";
  Key[LogScale] = "LogScalex";
  Def[LogScale] = "FALSE";
  Key[IndexPath] = "IndexPath";
  Def[IndexPath] = "";
  Key[UserDocsPath] = "UserDocsPath";
  Def[UserDocsPath] = "";
  Key[DocsPath] = "DocsPath";
  Def[DocsPath] = "";
  Key[LastQuotePlugin] = "LastQuotePlugin";
  Def[LastQuotePlugin] = "Yahoo";
  Key[MainWindowSize] = "/WindowSizes/Main";
  Def[MainWindowSize] = "792,570";
  Key[MainWindowPos] = "/WindowSizes/MainPos";
  Def[MainWindowPos] = "0,0";
  Key[PrefDlgWindowSize] = "/WindowSizes/PrefDialog";
  Def[PrefDlgWindowSize] = "480,380";
  Key[EditStockWindowSize] = "/WindowSizes/EditStock";
  Def[EditStockWindowSize] = "350,250";
  Key[HelpWindowSize] = "/WindowSizes/HelpWindow";
  Def[HelpWindowSize] = "750,530";
  Key[BackTesterWindowSize] = "/WindowSizes/BackTester";
  Def[BackTesterWindowSize] = "";
  Key[ScannerWindowSize] = "/WindowSizes/Scanner";
  Def[ScannerWindowSize] = "";
  Key[ScannerPath] = "ScannerPath";
  Def[ScannerPath] = "";
  Key[Version] = "Version";
  Def[Version] = "";  // I guess better to be empty
  Key[PlotSizes] = "/PlotSizes/";
  Def[PlotSizes] = "315,84,84";
  Key[ShowMenuBar] = "Preferences/ShowMenuBar";
  Def[ShowMenuBar] = "TRUE";
  Key[ShowExtraToolbar] = "Preferences/ShowExtraToolbar";
  Def[ShowExtraToolbar] = "TRUE";
  Key[COPath] = "COPath";
  Def[COPath] = "";
  Key[LocalIndicatorsPath] = "LocalIndicatorsPath";
  Def[LocalIndicatorsPath] = "";
  Key[FundamentalsPath] = "FundamentalsPath";
  Def[FundamentalsPath] = "";
  Key[CurrentChart] = "CurrentChart";
  Def[CurrentChart] = "";
  Key[IndicatorGroup] = "IndicatorGroup";
  Def[IndicatorGroup] = "Indicators";
  Key[QuotePluginStorage] = "QuotePluginStorage";
  Def[QuotePluginStorage] = "";
  Key[LastNewIndicator] = "LastNewIndicator";
  Def[LastNewIndicator] = "";
  Key[PSButtonCount] = "Preferences/PSButtonCount";
  Def[PSButtonCount] = "3";
  Key[PSButton] = "Preferences/PSButton";
  Def[PSButton] = "6";
  Key[ShowSidePanel] = "ShowSidePanel";
  Def[ShowSidePanel] = "TRUE";
  Key[ShowQuitBtn] = "Preferences/ShowQuitBtn";
  Def[ShowQuitBtn] = "TRUE";
  Key[ShowPrefBtn] = "Preferences/ShowPrefBtn";
  Def[ShowPrefBtn] = "TRUE";
  Key[ShowSidePanelBtn] = "Preferences/ShowSidePanelBtn";
  Def[ShowSidePanelBtn] = "TRUE";
  Key[ShowGridBtn] = "Preferences/ShowGridBtn";
  Def[ShowGridBtn] = "TRUE";
  Key[ShowScaleToScreenBtn] = "Preferences/ShowScaleToScreenBtn";
  Def[ShowScaleToScreenBtn] = "TRUE";
  Key[ShowCrosshairBtn] = "Preferences/ShowCrosshairBtn";
  Def[ShowCrosshairBtn] = "TRUE";
  Key[ShowPaperTradeBtn] = "Preferences/ShowPaperTradeBtn";
  Def[ShowPaperTradeBtn] = "TRUE";
  Key[ShowDrawModeBtn] = "Preferences/ShowDrawModeBtn";
  Def[ShowDrawModeBtn] = "TRUE";
  Key[ShowNewIndicatorBtn] = "Preferences/ShowNewIndicatorBtn";
  Def[ShowNewIndicatorBtn] = "TRUE";
  Key[ShowDataWindowBtn] = "Preferences/ShowDataWindowBtn";
  Def[ShowDataWindowBtn] = "TRUE";
  Key[ShowMainQuoteBtn] = "Preferences/ShowMainQuoteBtn";
  Def[ShowMainQuoteBtn] = "TRUE";
  Key[ShowHelpButton] = "Preferences/ShowHelpButton";
  Def[ShowHelpButton] = "TRUE";
  Key[ShowSlider] = "Preferences/ShowSlider";
  Def[ShowSlider] = "TRUE";
  Key[ShowBarsToLoadField] = "Preferences/ShowBarsToLoadField";
  Def[ShowBarsToLoadField] = "TRUE";
  Key[ShowBarSpSpinbox] = "Preferences/ShowBarSpSpinbox";
  Def[ShowBarSpSpinbox] = "TRUE";
  Key[ShowCmps1MBtn] = "Preferences/ShowCmps1MBtn";
  Def[ShowCmps1MBtn] = "TRUE";
//  Key[ShowCmps2MBtn] = "Preferences/ShowCmps2MBtn";
//  Def[ShowCmps2MBtn] = "FALSE";
  Key[ShowCmps5MBtn] = "Preferences/ShowCmps5MBtn";
  Def[ShowCmps5MBtn] = "TRUE";
  Key[ShowCmps10MBtn] = "Preferences/ShowCmps10MBtn";
  Def[ShowCmps10MBtn] = "FALSE";
  Key[ShowCmps15MBtn] = "Preferences/ShowCmps15MBtn";
  Def[ShowCmps15MBtn] = "TRUE";
//  Key[ShowCmps20MBtn] = "Preferences/ShowCmps20MBtn";
//  Def[ShowCmps20MBtn] = "FALSE";
  Key[ShowCmps30MBtn] = "Preferences/ShowCmps30MBtn";
  Def[ShowCmps30MBtn] = "FALSE";
  Key[ShowCmps1HBtn] = "Preferences/ShowCmps1HBtn";
  Def[ShowCmps1HBtn] = "TRUE";
//  Key[ShowCmps2HBtn] = "Preferences/ShowCmps2HBtn";
//  Def[ShowCmps2HBtn] = "FALSE";
//  Key[ShowCmps4HBtn] = "Preferences/ShowCmps4HBtn";
//  Def[ShowCmps4HBtn] = "TRUE";
  Key[ShowCmps1DBtn] = "Preferences/ShowCmps1DBtn";
  Def[ShowCmps1DBtn] = "TRUE";
  Key[ShowCmps1WBtn] = "Preferences/ShowCmps1WBtn";
  Def[ShowCmps1WBtn] = "TRUE";
  Key[ShowCmpsMNBtn] = "Preferences/ShowCmpsMNBtn";
  Def[ShowCmpsMNBtn] = "TRUE";
  Key[ShowCmpsComboBox] = "Preferences/ShowCmpsComboBox";
  Def[ShowCmpsComboBox] = "TRUE";
  Key[PrefColorCount] = "Preferences/PrefColorCount";
  Def[PrefColorCount] = "5";
  Key[PrefColor] = "Preferences/PrefColor";
  Def[PrefColor] = "#ffffff"; // white
  Key[ShowRecentCharts] = "Preferences/ShowRecentCharts";
  Def[ShowRecentCharts] = "TRUE";
  Key[LastAktiveNav] = "LastAktiveNav";
  Def[LastAktiveNav] = "";
  Key[LastGroupUsed] = "LastGroupUsed";
  Def[LastGroupUsed] = "";
  Key[LastChartDir] = "LastChartDir";
  Def[LastChartDir] = "";
  Key[LastIndicatorUsed] = "LastIndicatorUsed";
  Def[LastIndicatorUsed] = "";
  Key[LastDateTimeCOAdded] = "LastDateTimeCOAdded";
  Def[LastDateTimeCOAdded] = "";
  Key[MainToolBarPos] = "MainToolBarPos";
  Def[MainToolBarPos] = "";
  Key[ChartToolBarPos] = "ChartToolBarPos";
  Def[ChartToolBarPos] = "";
  Key[NavigatorPosition] = "NavigatorPosition";
  Def[NavigatorPosition] = "1"; // right hand side
  Key[NavigatorActiveButton] = "NavigatorActiveButton";
  Def[NavigatorActiveButton] = "0"; // chart panel
  Key[RecentChartsList] = "RecentChartsList";
  Def[RecentChartsList] = "";

  /* keep this as blueprint for easy add new ones
  Key[] = "";
  Def[] = "";
  */

}

RcFile::~RcFile()
{
}

//
//  public functions, no "string Parm" allowed
//  to prevent mismatch in the rc-file
//

void RcFile::loadData(Parm name, QString &value, const QString &s)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(s);

  value = Def[name];

  QSettings settings;
  value = settings.readEntry(k, value);
}

void RcFile::loadData(Parm name, QString &value, const int n)
{
  QString s;
  if (n > -1) s.append(QString::number(n));
  loadData(name, value, s);
}

void RcFile::loadData(Parm name, int &value, const int n)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  if (n > -1) k.append(QString::number(n));

  QString v = Def[name];
  value = v.toInt();

  QSettings settings;
  value = settings.readNumEntry(k, value);
}

void RcFile::loadData(Parm name, bool &value)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);

  if (Def[name] == QString("TRUE")) value = TRUE;
  else value = FALSE;

  QSettings settings;
  value = settings.readBoolEntry(k, value);
}

void RcFile::saveData(Parm name, QString &value, const QString &s)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  k.append(s);

  QSettings settings;
  settings.writeEntry(k, value);
}

void RcFile::saveData(Parm name, QString &value, const int n)
{
  QString s;
  if (n > -1) s.append(QString::number(n));
  saveData(name, value, s);
}

void RcFile::saveData(Parm name, int value, const int n)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);
  if (n > -1) k.append(QString::number(n));

  QSettings settings;
  settings.writeEntry(k, value);
}

void RcFile::saveData(Parm name, const bool value)
{
  QString k;
  k.append("/Qtstalker/");
  k.append(Key[name]);

  QSettings settings;
  settings.writeEntry(k, value);
}

//
// for special purposes
//
void RcFile::loadColor(Parm name, QColor &color)
{
  QString c;
  loadData(name, c);
  color.setNamedColor(c);
}

void RcFile::loadColor(Parm name, QColor &color, const int n)
{
  QString c;
  loadData(name, c, n);
  color.setNamedColor(c);
}

void RcFile::saveColor(Parm name, QColor &color)
{
  QString c = color.name();
  saveData(name, c);
}

void RcFile::saveColor(Parm name, QColor &color, const int n)
{
  QString c = color.name();
  saveData(name, c, n);
}

void RcFile::loadFont(Parm name, QFont &font)
{
  QString f;

  loadData(name, f);

  QStringList l = QStringList::split(",", f, FALSE);

  font.setFamily(l[0]);
  font.setPointSize(l[1].toInt());
  font.setWeight(l[2].toInt());
  font.setItalic(l[3].toInt());
  font.setBold(l[4].toInt());
}

void RcFile::saveFont(Parm name, QFont &font)
{
  QString f =font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic()) + "," +
              QString::number(font.bold());

  saveData(name, f);
}

void RcFile::loadSplitterSize(Parm name, QSplitter *sp,  const QString &n)
{
  QString s;
  loadData(name, s, n);

  QStringList stringList = QStringList::split(",", s, FALSE);
  Q3ValueList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) stringList.count(); loop++)
    sizeList[loop] = stringList[loop].toInt();

  sp->setSizes(sizeList);
}

void RcFile::saveSplitterSize(Parm name, QSplitter *sp, const QString &n)
{
  QStringList stringList;
  Q3ValueList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    stringList.append(QString::number(sizeList[loop]));

  QString s(stringList.join(","));
  saveData(name, s, n);
}

void RcFile::loadPoint(Parm name, QPoint &p, const int n)
{
  QString s;
  loadData(name, s, n);

  QStringList l = QStringList::split(",", s, FALSE);
  p.setX(l[0].toInt());
  p.setY(l[1].toInt());
}

void RcFile::savePoint(Parm name, const QPoint &p, const int n)
{
  QString s = QString::number(p.x()) + "," + QString::number(p.y());

  saveData(name, s, n);
}

void RcFile::loadSize(Parm name, QSize &sz, const int n)
{
  QString s;
  loadData(name, s, n);

  QStringList l = QStringList::split(",", s, FALSE);
  sz.setWidth(l[0].toInt());
  sz.setHeight(l[1].toInt());
}

void RcFile::saveSize(Parm name, const QSize &sz, const int n)
{
  QString s = QString::number(sz.width()) + "," + QString::number(sz.height());

  saveData(name, s, n);
}
