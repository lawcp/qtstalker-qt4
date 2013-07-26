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


#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qlibrary.h>
#include <q3dict.h>
#include <qsplitter.h>
#include "IndicatorPlugin.h"
#include "QuotePlugin.h"
#include "Setting.h"
#include "RcFile.h"

class Config
{
  public:
    enum Parm
    {
      Home,
      IndexPath,
      COPath,
      DataPath,
      FundamentalsPath,
      GroupPath,
      IndicatorPath,
      IndicatorPluginPath, // unused
      LocalIndicatorsPath,
      PortfolioPath,
      QuotePluginPath,
      ScannerPath,
      TestPath,
      UserDocsPath,
      AppFont,
      BackgroundColor,
      BarLength,
      Bars,
      BorderColor,
      Crosshairs,
      CurrentChart,
      DataPanelSize,
      DrawMode,
      Grid,
      GridColor,
      Group,
      Height,
      IndicatorGroup,
      LastNewIndicator,
      LastQuotePlugin,
      LogScale,
      Macro0, //unused
      Macro1, //unused
      Macro2, //unused
      Macro3, //unused
      Macro4, //unused
      Macro5, //unused
      Macro6, //unused
      Macro7, //unused
      Menubar,
      NavAreaSize,
      PS1Button,
      PS2Button,
      PS3Button,
      PaperTradeMode,
      Pixelspace,
      PlotFont,
      PlotSizes,
      QuotePluginStorage,
      ScaleToScreen,
      ShowUpgradeMessage, // unused
      Version,
      Width,
      X, Y
    };

    enum Indicator
    {
      Config_BARS,
      Config_CUS,
      Config_ExScript,
      Config_FI,
      Config_LMS,
      Config_LOWPASS,
      Config_PP,
      Config_SINWAV,
      Config_SZ,
      Config_THERM,
      Config_VFI,
      Config_VIDYA,
      Config_VOL
    };

    Config();
    ~Config();
    void setData(Parm, QString &);
    void setData(QString &, QString &);
    void getData(Parm, QString &);
    void getData(QString &, QString &);
    void loadSplitterSize(Parm, QSplitter *);
    void saveSplitterSize(Parm, QSplitter *);
    void getDirList(QString &, bool, QStringList &);
    void setup();

    void getIndicators(QString &, QStringList &);
    void getIndicator(QString &, Setting &);
    void deleteIndicator(QString &);
    void getIndicatorList(QStringList &);
    void setIndicator(QString &, Setting &);

    void getPluginList(Config::Parm, QStringList &);
    IndicatorPlugin * getIndicatorPlugin(QString &);
    QuotePlugin * getQuotePlugin(QString &);
    void closePlugins();
    void closePlugin(QString &);

    void copyIndicatorFile(QString &, QString &);
//    void checkUpgrade(); deprecated
    void check034Conversion();

  protected:
    Q3Dict<QLibrary> libs;
    Q3Dict<IndicatorPlugin> indicatorPlugins;
    Q3Dict<QuotePlugin> quotePlugins;
    QString version;
    QStringList indicatorList;
    QStringList indicatorList2;
    RcFile rcfile;
};

#endif
