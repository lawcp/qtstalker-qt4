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

#include "CC.h"
#include "FuturesData.h"
#include "Config.h"
#include "Bar.h"
#include "PrefDialog.h"
#include "DbPlugin.h"
#include <qdir.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <qmessagebox.h>


CC::CC ()
{
}

CC::~CC ()
{
}

void CC::getHistory (BarData *barData, QDateTime &startDate, QString &fs, bool adjustFlag,
                     int barRange, BarData::BarLength barLength)
{
  FuturesData fd;
  QString s;
  if (fd.setSymbol(fs))
  {
    qDebug("DbPlugin::getCCHistory: invalid futures symbol");
    return;
  }

  Config config;
  QString baseDir;
  config.getData(Config::DataPath, baseDir);
  baseDir.append("/Futures/");
  fd.getExchange(s);
  baseDir.append(s + "/");
  fd.getSymbol(s);
  baseDir.append(s);
  QDir dir(baseDir);
  if (! dir.exists(baseDir, TRUE))
    return;
  QStringList dirList = dir.entryList();

  QString lastChart;
  fd.getCurrentContract(startDate, lastChart);
  QString ey = lastChart.right(5);
  ey.truncate(4);

  QValueList<Bar> indexList;
  int indexCount = -1;

  int dirLoop = dirList.findIndex(lastChart);
  if (dirLoop == -1)
    dirLoop = dirList.count() - 1;
  lastChart = dirList[dirLoop];

  while (dirLoop > 1)
  {
    if (indexCount >= barRange)
      break;

    s = baseDir + "/" + dirList[dirLoop];

    DbPlugin tdb;
    if (tdb.openChart(s))
    {
      tdb.close();
      dirLoop--;
      lastChart = dirList[dirLoop];
      continue;
    }

    BarData *recordList = new BarData(s);
    tdb.setBarRange(barRange);
    tdb.setBarLength(barLength);
    tdb.getHistory(recordList, startDate);
    tdb.close();

    int loop;
    QDateTime dt = startDate;
    int lastBar = -1;
    bool dataFlag = FALSE;
    for (loop = recordList->count() - 1; loop > -1; loop--)
    {
      if (indexCount >= barRange)
        break;

      recordList->getDate(loop, dt);
      fd.getCurrentContract(dt, s);
      if (! s.compare(lastChart))
      {
        Bar bar;
        recordList->getBar(loop, bar);
        indexList.prepend(bar);
        indexCount++;
        startDate = dt;
        lastBar = loop;
        dataFlag = TRUE;
      }
    }

    if (dataFlag)
    {
      if (adjustFlag)
      {
        Bar bar;
        double t = 0;
        if (lastBar - 1 > -1)
          t = recordList->getClose(lastBar) - recordList->getClose(lastBar - 1);
        bar.setClose(t);
        bar.setEmptyFlag(TRUE);
        indexList.prepend(bar);
      }
    }

    delete recordList;

    dirLoop--;
    lastChart = dirList[dirLoop];
  }

  if (! adjustFlag)
  {
    int loop;
    for (loop = 0; loop < (int) indexList.count(); loop++)
    {
      Bar bar = indexList[loop];
      barData->appendRaw(bar);
    }

    return;
  }

  // adjust the data
  double adjust = 0;
  double t = 0;
  bool flag = FALSE;
  Bar prevBar;
  int loop;
  for (loop = 1; loop < (int) indexList.count(); loop++)
  {
    Bar bar = indexList[loop];

    if (bar.getEmptyFlag())
    {
      t = bar.getClose();
      flag = TRUE;
      continue;
    }

    if (flag)
    {
      adjust = prevBar.getClose() - bar.getClose();
      bar.setOpen(bar.getOpen() + t);
      bar.setHigh(bar.getHigh() + t);
      bar.setLow(bar.getLow() + t);
      bar.setClose(bar.getClose() + t);
      flag = FALSE;
      t = 0;
    }

    bar.setOpen(bar.getOpen() + adjust);
    bar.setHigh(bar.getHigh() + adjust);
    bar.setLow(bar.getLow() + adjust);
    bar.setClose(bar.getClose() + adjust);
    barData->appendRaw(bar);
    prevBar = bar;
  }
}

bool CC::createNew (QString &path, QString &symbol, QString &adjustment)
{
  FuturesData fd;
  QStringList l;
  fd.getSymbolList(l);

  QString pl = QObject::tr("Parms");
  QString fsl = QObject::tr("Futures Symbol");
  QString gl = QObject::tr("Gapless");

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("New CC"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addComboItem(fsl, pl, l, 0);
  dialog->addCheckItem(gl, pl, TRUE);
  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
  {
    dialog->getCombo(fsl, symbol);
    bool f = dialog->getCheck(gl);
    adjustment = QString::number(f);

    QDir dir;
    Config config;
    QString s;
    config.getData(Config::DataPath, s);
    s.append("/CC");
    if (! dir.exists(s))
    {
      if (! dir.mkdir(s, TRUE))
      {
        QMessageBox::information(0,
                                 QObject::tr("Qtstalker: Error"),
                                 QObject::tr("Could not create ~/.qtstalker/data/CC directory."));
        delete dialog;
        return FALSE;
      }
    }

    s.append("/" + symbol);
    if (dir.exists(s))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
                               QObject::tr("This CC already exists."));
      delete dialog;
      return FALSE;
    }
    path = s;

    delete dialog;
    return TRUE;
  }

  delete dialog;
  return FALSE;
}

bool CC::prefDialog (QString &adjustment)
{
  QString pl = QObject::tr("Parms");
  QString gl = QObject::tr("Gapless");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Edit CC"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addCheckItem(gl, pl, adjustment.toInt());
  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
  {
    bool f = dialog->getCheck(gl);
    adjustment = QString::number(f);
    delete dialog;
    return TRUE;
  }
  delete dialog;
  return FALSE;
}
