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

#include "Config.h"
#include "UpgradeMessage.h"
#include "TALIB.h"
#include "BARS.h"
#include "CUS.h"
#include "ExScript.h"
#include "FI.h"
#include "LMS.h"
#include "LOWPASS.h"
#include "PP.h"
#include "SINWAV.h"
#include "SZ.h"
#include "THERM.h"
#include "VFI.h"
#include "VIDYA.h"
#include "VOL.h"
#include "UTIL.h"
#include "SYMBOL.h"
#include <qobject.h>
#include <qdir.h>
#include <qlibrary.h>
#include <qsettings.h>
#include <qobject.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qapplication.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <stdlib.h>

#include "qtstalker_defines.h"

Config::Config ()
{
  libs.setAutoDelete(TRUE);
  indicatorPlugins.setAutoDelete(TRUE);
  quotePlugins.setAutoDelete(TRUE);
  // plugins of the following version shall be allowed to load
  version = PG_VERSION_PLUGIN;

  TALIB t;
  t.getIndicatorList(indicatorList);

  indicatorList2.append("BARS");
  indicatorList2.append("CUS");
  indicatorList2.append("ExScript");
  indicatorList2.append("FI");
  indicatorList2.append("LMS");
  indicatorList2.append("LOWPASS");
  indicatorList2.append("PP");
  indicatorList2.append("SINWAV");
  indicatorList2.append("SZ");
  indicatorList2.append("THERM");
  indicatorList2.append("VFI");
  indicatorList2.append("VIDYA");
  indicatorList2.append("VOL");

  int loop;
  for (loop = 0; loop < (int) indicatorList2.count(); loop++)
    indicatorList.append(indicatorList2[loop]);

  indicatorList.sort();
}

Config::~Config ()
{
  indicatorPlugins.clear();
  quotePlugins.clear();
  libs.clear();
}

void Config::setup ()
{
//  checkUpgrade(); // deprecated

  QDir dir(QDir::homeDirPath());
  dir.convertToAbs();
  QString home = dir.absPath();
  home.append("/.qtstalker");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home, TRUE))
      qDebug("Unable to create ~/.qtstalker directory.");
  }
  home.append("/data1");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1 directory.");
  }
  rcfile.saveData(RcFile::Home, home);

  QString s = home + "/index";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/index directory.");
  }
  s = home + "/index/index.db";
  rcfile.saveData(RcFile::IndexPath, s);
  s = home + "/index/co.db";
  rcfile.saveData(RcFile::COPath, s);
  s = home + "/index/li.db";
  rcfile.saveData(RcFile::LocalIndicatorsPath, s);
  s = home + "/index/fund.db";
  rcfile.saveData(RcFile::FundamentalsPath, s);

  s = home + "/data";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/data directory.");
  }
  rcfile.saveData(RcFile::DataPath, s);

  s = home + "/group";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/group directory.");
  }
  rcfile.saveData(RcFile::GroupPath, s);

  s = home + "/portfolio";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/portfolio directory.");
  }
  rcfile.saveData(RcFile::PortfolioPath, s);

  s = home + "/test";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/test directory.");
  }
  rcfile.saveData(RcFile::TestPath, s);

  s = home + "/scanner";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/scanner directory.");
  }
  rcfile.saveData(RcFile::ScannerPath, s);

  s = home + "/indicator";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/indicator directory.");
  }
  rcfile.saveData(RcFile::IndicatorPath, s);

  s = home + "/indicator/Indicators";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/indicator/Indicators directory.");
    else
    {
      QString str("Indicators");
      rcfile.saveData(RcFile::IndicatorGroup, str); // set the new default template
      QString ts = QString("cp %1/qtstalker/indicator/* %2").arg(INSTALL_DATA_DIR).arg(s);
      if (system(ts.latin1()))
        qDebug("setDefaultIndicators::copyFiles: error copying examples indicator files");
    }
  }

  s = home + "/plugin";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/plugin directory.");
  }

  s.append("/quote");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/plugin/quote directory.");
  }
  rcfile.saveData(RcFile::QuotePluginStorage, s);

  s = home + "/docs";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/docs directory.");
  }
  rcfile.saveData(RcFile::UserDocsPath, s);
  s.append("/indicator");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data1/docs/indicator directory.");
  }

  s = INSTALL_DOCS_DIR;
  s += "/qtstalker/html";
  rcfile.saveData(RcFile::DocsPath, s);

  s = INSTALL_LIB_DIR;
#ifdef MINGW
  s = ""; //in other directory, library can not be loaded at runtime. could be improved by handlinge path settings
#else
  s += "/qtstalker/quote";
#endif
  rcfile.saveData(RcFile::QuotePluginPath, s);

  check034Conversion();

  // set the version #
  rcfile.saveData(RcFile::Version, version);
}

void Config::getData (Parm p, QString &s)
{
  QSettings settings;

  s.truncate(0);
  switch (p)
  {
    case Home:
      s = settings.readEntry("/Qtstalker/Home");
      break;
    case DataPath:
      s = settings.readEntry("/Qtstalker/DataPath");
      break;
    case GroupPath:
      s = settings.readEntry("/Qtstalker/GroupPath");
      break;
    case PortfolioPath:
      s = settings.readEntry("/Qtstalker/PortfolioPath");
      break;
    case TestPath:
      s = settings.readEntry("/Qtstalker/TestPath");
      break;
    case ScannerPath:
      s = settings.readEntry("/Qtstalker/ScannerPath");
      break;
    case IndicatorPath:
      s = settings.readEntry("/Qtstalker/IndicatorPath");
      break;
    case BarLength:
      s = settings.readEntry("/Qtstalker/Compression", "6");
      break;
    case Grid:
      s = settings.readEntry("/Qtstalker/Grid", "1");
      break;
    case Bars:
      s = settings.readEntry("/Qtstalker/Bars", "275");
      break;
    case BackgroundColor:
      s = settings.readEntry("/Qtstalker/BackgroundColor", "black");
      break;
    case BorderColor:
      s = settings.readEntry("/Qtstalker/BorderColor", "white");
      break;
    case GridColor:
      s = settings.readEntry("/Qtstalker/GridColor", "#626262");
      break;
    case ScaleToScreen:
      s = settings.readEntry("/Qtstalker/ScaleToScreen", "1");
      break;
    case QuotePluginPath:
      s = settings.readEntry("/Qtstalker/QuotePluginPath");
      break;
    case Group:
      s = settings.readEntry("/Qtstalker/Group");
      break;
    case PlotFont:
      s = settings.readEntry("/Qtstalker/PlotFont", "DejaVu Sans,10,50,0,0");
      break;
    case AppFont:
      s = settings.readEntry("/Qtstalker/AppFont", "DejaVu Sans,10,50,0,0");
      break;
    case NavAreaSize:
      s = settings.readEntry("/Qtstalker/NavAreaSize", "20");
      break;
    case LogScale:
      s = settings.readEntry("/Qtstalker/LogScale", "0");
      break;
    case Height:
      s = settings.readEntry("/Qtstalker/Height", "640");
      break;
    case Width:
      s = settings.readEntry("/Qtstalker/Width", "480");
      break;
    case X:
      s = settings.readEntry("/Qtstalker/X", "0");
      break;
    case Y:
      s = settings.readEntry("/Qtstalker/Y", "0");
      break;
    case Crosshairs:
      s = settings.readEntry("/Qtstalker/Crosshairs", "1");
      break;
    case DrawMode:
      s = settings.readEntry("/Qtstalker/DrawMode", "0");
      break;
    case UserDocsPath:
      s = settings.readEntry("/Qtstalker/UserDocsPath");
      break;
    case LastQuotePlugin:
      s = settings.readEntry("/Qtstalker/LastQuotePlugin");
      break;
    case PlotSizes:
      s = settings.readEntry("/Qtstalker/PlotSizes");
      break;
    case DataPanelSize:
      s = settings.readEntry("/Qtstalker/DataPanelSize");
      break;
    case Menubar:
      s = settings.readEntry("/Qtstalker/Menubar", "1");
      break;
    case IndicatorGroup:
      s = settings.readEntry("/Qtstalker/IndicatorGroup", "Indicators");
      break;
    case QuotePluginStorage:
      s = settings.readEntry("/Qtstalker/QuotePluginStorage");
      break;
    case Version:
      s = settings.readEntry("/Qtstalker/Version");
      break;
    case ShowUpgradeMessage:
      s = settings.readEntry("/Qtstalker/ShowUpgradeMessage", "1");
      break;
    case LastNewIndicator:
      s = settings.readEntry("/Qtstalker/LastNewIndicator", "MA");
      break;
    case PaperTradeMode:
      s = settings.readEntry("/Qtstalker/PaperTradeMode", "0");
      break;
    case Pixelspace:
      s = settings.readEntry("/Qtstalker/Pixelspace", "4");
      break;
    case PS1Button:
      s = settings.readEntry("/Qtstalker/PS1Button", "2");
      break;
    case PS2Button:
      s = settings.readEntry("/Qtstalker/PS2Button", "4");
      break;
    case PS3Button:
      s = settings.readEntry("/Qtstalker/PS3Button", "6");
      break;
    case IndexPath:
      s = settings.readEntry("/Qtstalker/IndexPath");
      break;
    case COPath:
      s = settings.readEntry("/Qtstalker/COPath");
      break;
    case LocalIndicatorsPath:
      s = settings.readEntry("/Qtstalker/LocalIndicatorsPath");
      break;
    case FundamentalsPath:
      s = settings.readEntry("/Qtstalker/FundamentalsPath");
      break;
    case CurrentChart:
      s = settings.readEntry("/Qtstalker/CurrentChart");
      break;
    default:
      break;
  }
}

void Config::getData (QString &p, QString &s)
{
  QSettings settings;
  s = settings.readEntry("/Qtstalker/" + p);
}

void Config::setData (Parm p, QString &d)
{
  QSettings settings;

  switch (p)
  {
    case Home:
      settings.writeEntry("/Qtstalker/Home", d);
      break;
    case DataPath:
      settings.writeEntry("/Qtstalker/DataPath", d);
      break;
    case GroupPath:
      settings.writeEntry("/Qtstalker/GroupPath", d);
      break;
    case PortfolioPath:
      settings.writeEntry("/Qtstalker/PortfolioPath", d);
      break;
    case TestPath:
      settings.writeEntry("/Qtstalker/TestPath", d);
      break;
    case ScannerPath:
      settings.writeEntry("/Qtstalker/ScannerPath", d);
      break;
    case IndicatorPath:
      settings.writeEntry("/Qtstalker/IndicatorPath", d);
      break;
    case BarLength:
      settings.writeEntry("/Qtstalker/Compression", d);
      break;
    case Grid:
      settings.writeEntry("/Qtstalker/Grid", d);
      break;
    case Bars:
      settings.writeEntry("/Qtstalker/Bars", d);
      break;
    case BackgroundColor:
      settings.writeEntry("/Qtstalker/BackgroundColor", d);
      break;
    case BorderColor:
      settings.writeEntry("/Qtstalker/BorderColor", d);
      break;
    case GridColor:
      settings.writeEntry("/Qtstalker/GridColor", d);
      break;
    case ScaleToScreen:
      settings.writeEntry("/Qtstalker/ScaleToScreen", d);
      break;
    case QuotePluginPath:
      settings.writeEntry("/Qtstalker/QuotePluginPath", d);
      break;
    case Group:
      settings.writeEntry("/Qtstalker/Group", d);
      break;
    case PlotFont:
      settings.writeEntry("/Qtstalker/PlotFont", d);
      break;
    case AppFont:
      settings.writeEntry("/Qtstalker/AppFont", d);
      break;
    case NavAreaSize:
      settings.writeEntry("/Qtstalker/NavAreaSize", d);
      break;
    case LogScale:
      settings.writeEntry("/Qtstalker/LogScale", d);
      break;
    case Height:
      settings.writeEntry("/Qtstalker/Height", d);
      break;
    case Width:
      settings.writeEntry("/Qtstalker/Width", d);
      break;
    case X:
      settings.writeEntry("/Qtstalker/X", d);
      break;
    case Y:
      settings.writeEntry("/Qtstalker/Y", d);
      break;
    case Crosshairs:
      settings.writeEntry("/Qtstalker/Crosshairs", d);
      break;
    case DrawMode:
      settings.writeEntry("/Qtstalker/DrawMode", d);
      break;
    case UserDocsPath:
      settings.writeEntry("/Qtstalker/UserDocsPath", d);
      break;
    case LastQuotePlugin:
      settings.writeEntry("/Qtstalker/LastQuotePlugin", d);
      break;
    case PlotSizes:
      settings.writeEntry("/Qtstalker/PlotSizes", d);
      break;
    case DataPanelSize:
      settings.writeEntry("/Qtstalker/DataPanelSize", d);
      break;
    case Menubar:
      settings.writeEntry("/Qtstalker/Menubar", d);
      break;
    case IndicatorGroup:
      settings.writeEntry("/Qtstalker/IndicatorGroup", d);
      break;
    case QuotePluginStorage:
      settings.writeEntry("/Qtstalker/QuotePluginStorage", d);
      break;
    case Version:
      settings.writeEntry("/Qtstalker/Version", d);
      break;
    case ShowUpgradeMessage:
      settings.writeEntry("/Qtstalker/ShowUpgradeMessage", d);
      break;
    case LastNewIndicator:
      settings.writeEntry("/Qtstalker/LastNewIndicator", d);
      break;
    case PaperTradeMode:
      settings.writeEntry("/Qtstalker/PaperTradeMode", d);
      break;
    case Pixelspace:
      settings.writeEntry("/Qtstalker/Pixelspace", d);
      break;
    case PS1Button:
      settings.writeEntry("/Qtstalker/PS1Button", d);
      break;
    case PS2Button:
      settings.writeEntry("/Qtstalker/PS2Button", d);
      break;
    case PS3Button:
      settings.writeEntry("/Qtstalker/PS3Button", d);
      break;
    case IndexPath:
      settings.writeEntry("/Qtstalker/IndexPath", d);
      break;
    case COPath:
      settings.writeEntry("/Qtstalker/COPath", d);
      break;
    case LocalIndicatorsPath:
      settings.writeEntry("/Qtstalker/LocalIndicatorsPath", d);
      break;
    case FundamentalsPath:
      settings.writeEntry("/Qtstalker/FundamentalsPath", d);
      break;
    case CurrentChart:
      settings.writeEntry("/Qtstalker/CurrentChart", d);
      break;
    default:
      break;
  }
}

void Config::setData (QString &p, QString &d)
{
  QSettings settings;
  settings.writeEntry("/Qtstalker/" + p, d);
}

void Config::loadSplitterSize (Parm p, QSplitter *sp)
{
  QString s;
  getData(p, s);

  QStringList stringList = QStringList::split(",", s, FALSE);
  Q3ValueList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) stringList.count(); loop++)
    sizeList[loop] = stringList[loop].toInt();

  sp->setSizes(sizeList);
}

void Config::saveSplitterSize (Parm p, QSplitter *sp)
{
  QStringList stringList;
  Q3ValueList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    stringList.append(QString::number(sizeList[loop]));

  QString s(stringList.join(","));
  setData(p,s);
}

void Config::getIndicators (QString &d, QStringList &l)
{
  QString s;
  getData(IndicatorPath, s);
  s.append("/" + d);
  l.clear();
  getDirList(s, TRUE, l);
}

void Config::getIndicator (QString &d, Setting &set)
{
  QFile f(d);
  if (! f.open(QIODevice::ReadOnly))
  {
    qDebug("Config::getIndicator:can't open indicator file %s", d.latin1());
    return;
  }
  Q3TextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    QStringList l = QStringList::split("=", s, FALSE);
    if (l.count() < 2)
      continue;

    s = s.remove(0, l[0].length() + 1); // do this in case the data portion has an = sign in it.
    set.setData(l[0], s);
  }

  f.close();
}

void Config::setIndicator (QString &d, Setting &set)
{
  QFile f(d);
  if (! f.open(QIODevice::WriteOnly))
  {
    qDebug("Config::setIndicator:can't write to indicator file %s", d.latin1());
    return;
  }
  Q3TextStream stream(&f);

  int loop;
  QStringList l;
  set.getKeyList(l);
  QString k;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    set.getData(l[loop], k);
    stream << l[loop] << "=" << k << "\n";
  }

  f.close();
}

void Config::deleteIndicator (QString &d)
{
  QDir dir;
  dir.remove(d);
}

void Config::getDirList (QString &path, bool flag, QStringList &l)
{
  l.clear();
  QDir dir(path);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (! fi.isDir())
    {
      if (flag)
        l.append(fi.absFilePath());
      else
        l.append(dir[loop]);
    }
  }
}

void Config::getPluginList (Config::Parm d, QStringList &l2)
{
  l2.clear();
  QStringList l;
  QString s;
  getData(d, s);
  getDirList(s, TRUE, l);

  if (! l.count())
    return;

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    if (! fi.fileName().contains(version))
    {
      qDebug("Config::getPluginList:wrong plugin version: %s , I need version %s",fi.fileName().ascii(), version.ascii());
      continue;
    }

#ifdef MINGW
    if (! fi.extension( FALSE ).contains("dll",Qt::CaseInsensitive))
        continue;
#endif


    s = fi.baseName(FALSE);
#ifndef MINGW
    s.remove(0, 3); //for linux/unix, need to remove the 'lib' part
#endif
    l2.append(s);
  }
}

void Config::getIndicatorList (QStringList &l)
{
  l = indicatorList;
}

IndicatorPlugin * Config::getIndicatorPlugin (QString &p)
{
  IndicatorPlugin *plug = indicatorPlugins[p];
  if (plug)
    return plug;

  int i = indicatorList2.findIndex(p);
  switch (i)
  {
    case Config_BARS:
      plug = new BARS;
      break;
    case Config_CUS:
      plug = new CUS;
      break;
    case Config_ExScript:
      plug = new ExScript;
      break;
    case Config_FI:
      plug = new FI;
      break;
    case Config_LMS:
      plug = new LMS;
      break;
    case Config_LOWPASS:
      plug = new LOWPASS;
      break;
    case Config_PP:
      plug = new PP;
      break;
    case Config_SINWAV:
      plug = new SINWAV;
      break;
    case Config_SZ:
      plug = new SZ;
      break;
    case Config_THERM:
      plug = new THERM;
      break;
    case Config_VFI:
      plug = new VFI;
      break;
    case Config_VIDYA:
      plug = new VIDYA;
      break;
    case Config_VOL:
      plug = new VOL;
      break;
    default:
      if (! p.compare("UTIL"))
        plug = new UTIL;
      else
      {
        if (! p.compare("SYMBOL"))
          plug = new SYMBOL;
        else
          plug = new TALIB;
      }
      break;
  }

  if (! plug)
    qDebug("%s", p.latin1());
  else
    indicatorPlugins.replace(p, plug);
  return plug;
}

QuotePlugin * Config::getQuotePlugin (QString &p)
{
  QuotePlugin *plug = quotePlugins[p];
  if (plug)
    return plug;

  QString s;
  getData(QuotePluginPath, s);
#ifdef MINGW
  #ifdef DEBUG
    s.append(p + "." + version + "D.dll");
  #else
    s.append(p + "." + version + ".dll");
  #endif
#else
  s.append("/lib" + p + "." + version);
#endif

  QLibrary *lib = new QLibrary(s);
  QuotePlugin *(*so)() = 0;
  so = (QuotePlugin *(*)()) lib->resolve("createQuotePlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    quotePlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getQuotePlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

void Config::closePlugins ()
{
  indicatorPlugins.clear();
  quotePlugins.clear();
  libs.clear();
}

void Config::closePlugin (QString &d)
{
  indicatorPlugins.remove(d);
  quotePlugins.remove(d);
  libs.remove(d);
}

void Config::copyIndicatorFile (QString &d, QString &d2)
{
  QFile f(d);
  if (! f.open(QIODevice::ReadOnly))
  {
    qDebug("Config::copyFile:can't open input file %s", d.latin1());
    return;
  }
  Q3TextStream stream(&f);

  QFile f2(d2);
  if (! f2.open(QIODevice::WriteOnly))
  {
    qDebug("Config::copyFile:can't open output file %s", d2.latin1());
    f.close();
    return;
  }
  Q3TextStream stream2(&f2);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    stream2 << s << "\n";
  }
  stream2 << "enable=1\n";

  f.close();
  f2.close();
}

// Everyone should have upgraded by now, this is over 2 years old.
/*
void Config::checkUpgrade ()
{
  QDir dir(QDir::homeDirPath());
  dir.convertToAbs();
  QString s = dir.absPath() + "/Qtstalker";
  if (! dir.exists(s))
    return;

  // check if we need to delete the old qtstalkerrc file before we upgrade
  s = dir.absPath() + "/.qtstalker";
  if (! dir.exists(s))
  {
    s = dir.absPath() + "/.qt/qtstalkerrc";
    dir.remove(s, TRUE);
  }

  getData(ShowUpgradeMessage, s);
  if (! s.toInt())
    return;

  UpgradeMessage *dialog = new UpgradeMessage(UpgradeMessage::V031);
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    bool flag = dialog->getStatus();
    if (flag)
      s = QString::number(0);
    else
      s = QString::number(1);

    setData(ShowUpgradeMessage, s);
  }

  delete dialog;
}
*/

void Config::check034Conversion ()
{
  QString s;
  rcfile.loadData(RcFile::Version, s);
  if (! s.length())
    return;

  if (s.toDouble() >= 0.34)
    return;

  UpgradeMessage *dialog = new UpgradeMessage(UpgradeMessage::V034);
  dialog->exec();
  delete dialog;
}
