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

#include "NYBOT.h"
#include "Setting.h"
#include "Bar.h"
#include "DBIndexItem.h"
#include "Exchange.h"
#include <qfile.h>
#include <q3textstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include <q3network.h>
#include <qlabel.h>


NYBOT::NYBOT ()
{
  pluginName = "NYBOT";
  helpFile = "nybot.html";
  cancelFlag = FALSE;

  connect(this, SIGNAL(signalGetFileDone(bool)), this, SLOT(fileDone(bool)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));

  buildGui();
  loadSettings();
  q3InitNetworkProtocols();
  resize(640, 480);
}

NYBOT::~NYBOT ()
{
  plug.close();
  saveSettings();
}

void NYBOT::buildGui ()
{
  setCaption(tr("NYBOT Quotes"));

  QLabel *label = new QLabel(tr("Date"), baseWidget);
  grid->addWidget(label, 0, 0);

  date = new Q3DateEdit(QDate::currentDate(), baseWidget);
  date->setAutoAdvance(TRUE);
  date->setOrder(Q3DateEdit::YMD);
  grid->addWidget(date, 0, 1);

  QDate dt = QDate::currentDate();
  if (dt.dayOfWeek() == 6)
    dt = dt.addDays(-1);
  else
  {
    if (dt.dayOfWeek() == 7)
      dt = dt.addDays(-2);
  }
  date->setDate(dt);
}

void NYBOT::update ()
{
  plug.close();
  errorLoop = 0;

  config.getData(Config::Home, file);
  file.append("/nybotDownload");

  // http://www.nybot.com/reports/dmrs/download/downloadAllSelection.asp?commodityType=future
  // http://www.nybot.com/reports/dmrs/files/2005,05,13_ALL_futures.csv
  url = "http://www.nybot.com/reports/dmrs/files/";
  QDate dt = date->date();
  url.append(dt.toString("yyyy,MM,dd"));
  url.append("_ALL_futures.csv");
  getFile(url);
}

void NYBOT::fileDone (bool d)
{
  if (d)
  {
    QString ss(tr("Network error"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }

  QFile f(file);
  if (! f.open(QIODevice::WriteOnly))
  {
    QString ss(tr("Cant write to file"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }
  Q3TextStream stream(&f);

  stream << data;
  f.close();

  parse();
}

void NYBOT::timeoutError ()
{
  errorLoop++;
  if (errorLoop == retrySpin->value())
  {
    QString ss(tr("Timeout: retry limit"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }
  else
  {
    QString s = tr("Timeout: retry ") + QString::number(errorLoop + 1);
    printStatusLogMessage(s);
    getFile(url);
  }
}

void NYBOT::parse ()
{
  if (cancelFlag)
    return;

  QFile f(file);
  if (! f.open(QIODevice::ReadOnly))
    return;
  Q3TextStream stream(&f);

  QString ts = stream.readLine();
  QStringList keys = QStringList::split( '\n', substituteSeparator( ts, ',', '\n' ), FALSE);

  while(stream.atEnd() == 0)
  {
    ts = stream.readLine();
    QStringList l = QStringList::split( '\n', substituteSeparator( ts, ',', '\n' ), FALSE);

    if (l.count() != keys.count())
      continue;

    Setting data;
    int loop2;
    for (loop2 = 0; loop2 < (int) keys.count(); loop2++)
      data.setData(keys[loop2], l[loop2]);

    // symbol
    QString symbol;
    QString ts = "commoditySymbol";
    data.getData(ts, symbol);
    symbol = symbol.stripWhiteSpace();
    if (symbol.length() == 0)
      continue;

    if (! symbol.compare("CC") || ! symbol.compare("CR") || ! symbol.compare("CT") ||
        ! symbol.compare("DX") || ! symbol.compare("KC") || ! symbol.compare("OJ") ||
	! symbol.compare("SB") || ! symbol.compare("YX"))
    {
    }
    else
      continue;

    // date
    QString date;
    ts = "tradeDate";
    data.getData(ts, date);
    date.append("000000");

    Bar bar;
    if (bar.setDate(date))
    {
      QString ss = tr("Bad date") + " " + date;
      printStatusLogMessage(ss);
      continue;
    }

    ts = "dailyOpenPrice1";
    QString s;
    data.getData(ts, s);
    if (s.toFloat() == 0)
    {
      ts = "dailyOpenPrice2";
      data.getData(ts, s);
    }
    if (setTFloat(s, FALSE))
      continue;
    else
      bar.setOpen(tfloat);

    ts = "dailyHigh";
    data.getData(ts, s);
    if (setTFloat(s, FALSE))
      continue;
    else
      bar.setHigh(tfloat);

    ts = "dailyLow";
    data.getData(ts, s);
    if (setTFloat(s, FALSE))
      continue;
    else
      bar.setLow(tfloat);

    ts = "settlementPrice";
    data.getData(ts, s);
    if (setTFloat(s, FALSE))
      continue;
    else
      bar.setClose(tfloat);

    ts = "tradeVolume";
    data.getData(ts, s);
    if (setTFloat(s, FALSE))
      continue;
    else
      bar.setVolume(tfloat);

    ts = "openInterest";
    data.getData(ts, s);
    if (setTFloat(s, FALSE))
      continue;
    else
      bar.setOI((int) tfloat);

    if (symbol.compare("CC"))
    {
      bar.setOpen(bar.getOpen() / 100);
      bar.setHigh(bar.getHigh() / 100);
      bar.setLow(bar.getLow() / 100);
      bar.setClose(bar.getClose() / 100);
    }

    if (bar.verify())
      continue;

    //futures month
    ts = "contractMonth";
    data.getData(ts, s);
    QString year = s.left(4);
    QString month = s.right(2);
    QString fmonth;

    switch (month.toInt())
    {
      case 1:
        fmonth = "F";
        break;
      case 2:
        fmonth = "G";
        break;
      case 3:
        fmonth = "H";
        break;
      case 4:
        fmonth = "J";
        break;
      case 5:
        fmonth = "K";
        break;
      case 6:
        fmonth = "M";
        break;
      case 7:
        fmonth = "N";
        break;
      case 8:
        fmonth = "Q";
        break;
      case 9:
        fmonth = "U";
        break;
      case 10:
        fmonth = "V";
        break;
      case 11:
        fmonth = "X";
        break;
      case 12:
        fmonth = "Z";
        break;
      default:
        break;
    }

    if (fd.setSymbol(symbol))
      continue;

    if (year.length())
    {
      symbol.append(year);

      if (fmonth.length())
        symbol.append(fmonth);
      else
        continue;
    }
    else
      continue;

    s = "Futures/";
    QString s2;
    fd.getExchange(s2);
    s.append(s2 + "/");
    fd.getSymbol(s2);
    s.append(s2);
    QString path;
    createDirectory(s, path);
    if (! path.length())
    {
      QString ss(tr("Unable to create futures directory"));
      printStatusLogMessage(ss);
      return;
    }

    s = path + "/" + symbol;
    if (plug.open(s, chartIndex))
    {
      QString ss(tr("Could not open db"));
      printStatusLogMessage(ss);
      return;
    }

    DBIndexItem item;
    chartIndex->getIndexItem(symbol, item);
    item.getSymbol(s);
    if (! s.length())
    {
      if (plug.createNewFutures())
        return;

      chartIndex->getIndexItem(symbol, item);

      s = QString::number(Exchange::NYBOT);
      item.setExchange(s);

      item.setQuotePlugin(pluginName);

      chartIndex->setIndexItem(symbol, item);
    }

    plug.setBar(bar);

    plug.close();

    emit signalWakeup();
  }

  f.close();

  downloadComplete();
  if (cancelFlag)
  {
    cancelFlag = FALSE;
    QString ss(tr("Update cancelled"));
    printStatusLogMessage(ss);
  }
  else
  {
    QString ss(tr("Done"));
    printStatusLogMessage(ss);
  }
}

void NYBOT::cancelUpdate ()
{
  cancelFlag = TRUE;
}

void NYBOT::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/NYBOT plugin");

  QString s = settings.readEntry("/Retry", "3");
  retrySpin->setValue(s.toInt());

  s = settings.readEntry("/Timeout", "15");
  timeoutSpin->setValue(s.toInt());

  settings.endGroup();
}

void NYBOT::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/NYBOT plugin");
  settings.writeEntry("/Retry", retrySpin->text());
  settings.writeEntry("/Timeout", timeoutSpin->text());
  settings.endGroup();
}

//**********************************************************
//**********************************************************
//**********************************************************

QuotePlugin * createQuotePlugin ()
{
  NYBOT *o = new NYBOT;
  return ((QuotePlugin *) o);
}


