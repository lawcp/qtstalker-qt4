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

#include "CME.h"
#include "Bar.h"
#include "DBIndexItem.h"
#include "Exchange.h"
#include <qfile.h>
#include <q3textstream.h>
#include <q3network.h>
#include <qdir.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include <qlabel.h>


CME::CME ()
{
  pluginName = "CME";
  symbolCombo = 0;
  helpFile = "cme.html";
  downloadIndex = 0;
  cancelFlag = FALSE;

  fd.getCMESymbolList(symbolList);

  connect(this, SIGNAL(signalCopyFileDone(QString)), this, SLOT(fileDone(QString)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));

  config.getData(Config::Home, file);
  file.append("/download");

  q3InitNetworkProtocols();
  buildGui();
  loadSettings();

  resize(640, 480);
}

CME::~CME ()
{
  plug.close();
  saveSettings();
}

void CME::update ()
{
  plug.close();
  urlList.clear();
  symbolLoop = 0;
  errorLoop = 0;

  if (! methodCombo->currentText().compare("Today"))
  {
    urlList.append("ftp://ftp.cme.com//pub/settle/stlags");
    urlList.append("ftp://ftp.cme.com//pub/settle/stlcur");
    urlList.append("ftp://ftp.cme.com//pub/settle/stleqt");
    urlList.append("ftp://ftp.cme.com//pub/settle/stlint");
  }
  else
  {
    downloadIndex = 0;
    QString s;
    config.getData(Config::Home, s);

    // remove any old files
    QDir dir(s);
    int loop;
    for (loop = 2; loop < (int) dir.count(); loop++)
    {
      QString t = dir.absPath() + "/" + dir[loop];
      QFileInfo fi(t);
      if (fi.isDir())
        continue;

      if (! fi.extension(TRUE).compare("zip"))
      {
        dir.remove(fi.absFilePath());
        continue;
      }

      if (! fi.extension(TRUE).compare("eod"))
        dir.remove(fi.absFilePath());
    }

    file2 = s;
    s.append("/download.zip");
    file = s;

    s = "ftp://ftp.cme.com//pub/hist_eod/";
    s.append(symbolCombo->currentText().lower());
    s.append("ytd.zip");
    urlList.append(s);
  }

  startDownload();
}

void CME::startDownload ()
{
  QString s = tr("Downloading ");
  s.append(urlList[symbolLoop]);
  printStatusLogMessage(s);

  copyFile(urlList[symbolLoop], file);
}

void CME::fileDone (QString d)
{
  if (d.length())
  {
    qDebug(d.latin1());
    printStatusLogMessage(d);
    printStatusLogMessage(stringDone);
    downloadComplete();
    return;
  }

  if (! methodCombo->currentText().compare("Today"))
  {
    parseToday();

    symbolLoop++;

    if (symbolLoop >= (int) urlList.count())
    {
      printStatusLogMessage(stringDone);
      downloadComplete();
      return;
    }

    startDownload();
  }
  else
  {
    parseHistory();
    printStatusLogMessage(stringDone);
    downloadComplete();
  }
}

void CME::timeoutError ()
{
  errorLoop++;
  if (errorLoop == retrySpin->value())
  {
    QString s(tr("Timeout: retry limit skipping file"));
    printStatusLogMessage(s);

    errorLoop = 0;

    if (! methodCombo->currentText().compare("Today"))
    {
      symbolLoop++;
      if (symbolLoop >= (int) urlList.count())
      {
        printStatusLogMessage(stringDone);
        downloadComplete();
        return;
      }

      startDownload();
    }
  }
  else
  {
    QString s = tr("Timeout: retry ") + QString::number(errorLoop + 1);
    printStatusLogMessage(s);
    startDownload();
  }
}

void CME::parseToday ()
{
  QFile f(file);
  if (! f.open(QIODevice::ReadOnly))
    return;
  Q3TextStream stream(&f);

  QString s = stream.readLine();
  s = s.stripWhiteSpace();
  QString s2 = "PRICES AS OF ";
  int p = s.indexOf(s2);
  s.remove(0, p + s2.length() - 1);
  QString date = s.left(8);
  QStringList dl = QStringList::split("/", date, FALSE);
  date = "20";
  date.append(dl[2]);
  date.append(dl[0]);
  date.append(dl[1]);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (s.contains("FC CME FEEDER CATTLE"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("FC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LB CME LUMBER FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LC CME LIVE CATTLE FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LH CME LEAN HOG FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LN");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("PB CME FROZEN PORK BELLY FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("PB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("AD CME AUSTRALIAN DOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("AD");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("NB CME BRITISH POUND FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("NB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("CD CME CANADIAN DOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("CD");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("EC CME EURO FX FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("EC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("JY CME JAPANESE YEN FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("JY");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("SF CME SWISS FRANC FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("SF");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ES CME E-MINI S&P 500 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ES");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("GI CME GSCI FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("GI");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ND CME NASDAQ-100 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ND");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("NQ CME E-MINI NASDAQ-100 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("NQ");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("SP CME S&P 500 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("SP");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ED CME EURODOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ED");
	saveTodayData(l);
      }
    }
  }

  f.close();
}

void CME::parseHistory ()
{
  QString s2 = file2;
  s2.append("/");
  s2.append(symbolCombo->currentText().lower());
  s2.append("ytd.eod");
  QDir dir(s2);
  dir.remove(s2);

  QString s = "unzip ";
  s.append(file);
  s.append(" -d ");
  s.append(file2);
  if (system(s))
    return;

  QFile f(s2);
  if (! f.open(QIODevice::ReadOnly))
  {
    QString s(tr("could not open parse history file"));
    printStatusLogMessage(s);
    return;
  }
  Q3TextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    Setting set;

    // date
    s2 = s.mid(31, 6);
    s2.prepend("20");
    s2.append("000000");
    QString ts = "Date";
    set.setData(ts, s2);

    // csymbol
    s2 = s.mid(37, 2);
    ts = "CSymbol";
    set.setData(ts, s2);

    // symbol
    s2 = s.mid(37, 2);
    s2.append("20");
    s2.append(s.mid(41, 2));

    QString month = s.mid(43, 2);
    switch (month.toInt())
    {
      case 1:
        month = "F";
        break;
      case 2:
        month = "G";
        break;
      case 3:
        month = "H";
        break;
      case 4:
        month = "J";
        break;
      case 5:
        month = "K";
        break;
      case 6:
        month = "M";
        break;
      case 7:
        month = "N";
        break;
      case 8:
        month = "Q";
        break;
      case 9:
        month = "U";
        break;
      case 10:
        month = "V";
        break;
      case 11:
        month = "X";
        break;
      default:
        month = "Z";
        break;
    }
    s2.append(month);
    ts = "Symbol";
    set.setData(ts, s2);

    ts = "Month";
    set.setData(ts, month);

    QString dec = s.mid(30, 1);

    // fix decimal for JY
    ts = "CSymbol";
    set.getData(ts, s2);
    if (! s2.compare("JY"))
      dec = QString::number(dec.toInt() - 2);

    // open
    s2 = s.mid(53, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    ts = "Open";
    set.setData(ts, s2);

    // high
    s2 = s.mid(73, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    ts = "High";
    set.setData(ts, s2);

    // low
    s2 = s.mid(83, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    ts = "Low";
    set.setData(ts, s2);

    // close
    s2 = s.mid(113, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    ts = "Close";
    set.setData(ts, s2);

    // volume
    s2 = s.mid(122, 7);
    ts = "Volume";
    set.setData(ts, s2);

    // oi
    s2 = s.mid(136, 7);
    ts = "OI";
    set.setData(ts, s2);

    parse(set);
  }

  f.close();
}

void CME::saveTodayData (QStringList &l)
{
  Setting set;

  QString symbol = l[0];
  symbol.append("20");
  QString s = l[2];
  s.remove(0, 3);
  symbol.append(s);

  s = l[2];
  s.truncate(3);
  QString month;
  while (1)
  {
    if (! s.compare("JAN"))
    {
      month = "F";
      break;
    }

    if (! s.compare("FEB"))
    {
      month = "G";
      break;
    }

    if (! s.compare("MAR"))
    {
      month = "H";
      break;
    }

    if (! s.compare("APR"))
    {
      month = "J";
      break;
    }

    if (! s.compare("MAY"))
    {
      month = "K";
      break;
    }

    if (! s.compare("JUN"))
    {
      month = "M";
      break;
    }

    if (! s.compare("JLY"))
    {
      month = "N";
      break;
    }

    if (! s.compare("AUG"))
    {
      month = "Q";
      break;
    }

    if (! s.compare("SEP"))
    {
      month = "U";
      break;
    }

    if (! s.compare("OCT"))
    {
      month = "V";
      break;
    }

    if (! s.compare("NOV"))
    {
      month = "X";
      break;
    }

    month = "Z";

    break;
  }
  symbol.append(month);

  QString ts = "CSymbol";
  set.setData(ts, l[0]);
  ts = "Symbol";
  set.setData(ts, symbol);

  s = l[1];
  s.append("000000");
  ts = "Date";
  set.setData(ts, s);

  ts = "Month";
  set.setData(ts, month);

  QString open = l[3];
  QString high = l[4];
  QString low = l[5];
  QString close = l[7];
  QString volume = l[9];
  QString oi = l[12];

  s = l[0];
  if (! s.compare("JY"))
  {
    setTFloat(open, FALSE);
    open = QString::number(tfloat);
    if (open.toFloat() > 9999)
      open.insert(1, ".");
    else
      open.prepend(".");

    setTFloat(high, FALSE);
    high = QString::number(tfloat);
    if (high.toFloat() > 9999)
      high.insert(1, ".");
    else
      high.prepend(".");

    setTFloat(low, FALSE);
    low = QString::number(tfloat);
    if (low.toFloat() > 9999)
      low.insert(1, ".");
    else
      low.prepend(".");

    setTFloat(close, FALSE);
    close = QString::number(tfloat);
    if (close.toFloat() > 9999)
      close.insert(1, ".");
    else
      close.prepend(".");
  }

  ts = "Open";
  set.setData(ts, open);
  ts = "High";
  set.setData(ts, high);
  ts = "Low";
  set.setData(ts, low);
  ts = "Close";
  set.setData(ts, close);
  ts = "Volume";
  set.setData(ts, volume);
  ts = "OI";
  set.setData(ts, oi);

  parse(set);
}

void CME::parse (Setting &data)
{
  QString s;
  QString ts = "CSymbol";
  data.getData(ts, s);
  if (fd.setSymbol(s))
    return;

  Bar bar;
  ts = "Date";
  data.getData(ts, s);
  if (bar.setDate(s))
  {
    QString ss = "Bad date " + s;
    printStatusLogMessage(ss);
    return;
  }

  // open
  ts = "Open";
  data.getData(ts, s);
  if (setTFloat(s, FALSE))
    return;
  else
    bar.setOpen(tfloat);

  // high
  ts = "High";
  data.getData(ts, s);
  if (setTFloat(s, FALSE))
    return;
  else
    bar.setHigh(tfloat);

  // low
  ts = "Low";
  data.getData(ts, s);
  if (setTFloat(s, FALSE))
    return;
  else
    bar.setLow(tfloat);

  // close
  ts = "Close";
  data.getData(ts, s);
  if (setTFloat(s, FALSE))
    return;
  else
    bar.setClose(tfloat);

  // volume
  ts = "Volume";
  data.getData(ts, s);
  if (setTFloat(s, FALSE))
    return;
  else
    bar.setVolume(tfloat);

  // oi
  ts = "OI";
  data.getData(ts, s);
  if (setTFloat(s, FALSE))
    return;
  else
    bar.setOI((int) tfloat);

  if (bar.verify())
    return;

  QString s2;
  fd.getSymbol(s2);
  s = "Futures/CME/" + s2;
  QString path;
  createDirectory(s, path);
  if (! path.length())
  {
    QString ss(tr("Unable to create futures directory"));
    printStatusLogMessage(ss);
    return;
  }

  ts = "Symbol";
  QString ts2;
  data.getData(ts, ts2);
//  s = tr("Updating ") + ts2;
//  printStatusLogMessage(s);

  data.getData(ts, ts2);
  s = path + "/" + ts2;
  if (plug.open(s, chartIndex))
  {
    QString ss(tr("Could not open db."));
    printStatusLogMessage(ss);
    return;
  }

  QFileInfo fi(s);
  QString fn = fi.fileName();

  DBIndexItem item;
  chartIndex->getIndexItem(fn, item);
  item.getSymbol(s);
  if (! s.length())
  {
    if (plug.createNewFutures())
      return;

    chartIndex->getIndexItem(fn, item);

    s = QString::number(Exchange::CME);
    item.setExchange(s);

    item.setQuotePlugin(pluginName);

    chartIndex->setIndexItem(fn, item);
  }

  plug.setBar(bar);

  plug.close();

  emit signalWakeup();
}

void CME::cancelUpdate ()
{
  if (op)
  {
    timer->stop();
    op->stop();
  }

  downloadComplete();
  printStatusLogMessage(stringCanceled);
}

void CME::buildGui ()
{
  setCaption(tr("CME Prefs"));

  methodList.append("Today");
  methodList.append("History");

  QLabel *label = new QLabel(tr("Method"), baseWidget);
  grid->addWidget(label, 0, 0);

  methodCombo = new QComboBox(baseWidget);
  methodCombo->insertStringList(methodList, -1);
  QObject::connect(methodCombo, SIGNAL(activated(const QString &)), this, SLOT(methodChanged(const QString &)));
  grid->addWidget(methodCombo, 0, 1);

  label = new QLabel(tr("Symbol"), baseWidget);
  grid->addWidget(label, 1, 0);

  symbolCombo = new QComboBox(baseWidget);
  symbolCombo->insertStringList(symbolList, -1);
  grid->addWidget(symbolCombo, 1, 1);

  methodChanged(methodCombo->currentText());
}

void CME::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CME plugin");

  QString s = settings.readEntry("/Method", "Today");
  methodCombo->setCurrentItem(methodList.findIndex(s));

  s = settings.readEntry("/Symbol", "AD");
  symbolCombo->setCurrentItem(symbolList.findIndex(s));

  s = settings.readEntry("/Retry", "3");
  retrySpin->setValue(s.toInt());

  s = settings.readEntry("/Timeout", "15");
  timeoutSpin->setValue(s.toInt());

  settings.endGroup();

  methodChanged(methodCombo->currentText());
}

void CME::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CME plugin");

  settings.writeEntry("/Method", methodCombo->currentText());
  settings.writeEntry("/Symbol", symbolCombo->currentText());
  settings.writeEntry("/Retry", retrySpin->text());
  settings.writeEntry("/Timeout", timeoutSpin->text());

  settings.endGroup();
}

void CME::methodChanged (const QString & d)
{
  if (! d.compare("Today"))
    symbolCombo->setEnabled(FALSE);
  else
    symbolCombo->setEnabled(TRUE);
}

//*************************************************************
//*************************************************************
//*************************************************************

QuotePlugin * createQuotePlugin ()
{
  CME *o = new CME;
  return ((QuotePlugin *) o);
}

