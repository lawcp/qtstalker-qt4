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

#include "Spread.h"
#include "Config.h"
#include "Bar.h"
#include "PrefDialog.h"
#include "DbPlugin.h"
#include <qdir.h>
#include <qdict.h>
#include <qinputdialog.h>
#include <qobject.h>
#include <qmessagebox.h>


Spread::Spread ()
{
}

Spread::~Spread ()
{
}

void Spread::getHistory (BarData *barData, QDateTime &startDate, QString &fs, QString &ss,
                     int barRange, BarData::BarLength barLength)
{
  // get the first symbol bars
  QString s;
  DbPlugin db;
  if (db.openChart(fs))
  {
    qDebug("DbPlugin::getSpreadHistory: cannot open first symbol chart");
    db.close();
    return;
  }

  BarData *bar = new BarData(fs);
  bar->setBarLength(barLength);
  db.setBarRange(barRange);
  db.getHistory(bar, startDate);
  db.close();

  // get the second symbol bars
  if (db.openChart(ss))
  {
    qDebug("DbPlugin::getSpreadHistory: cannot open second symbol chart");
    db.close();
    delete bar;
    return;
  }

  BarData *bar2 = new BarData(ss);
  bar2->setBarLength(barLength);
  db.setBarRange(barRange);
  db.getHistory(bar2, startDate);
  db.close();

  // create lookup dict for first symbol bars
  QDict<Bar> lookup;
  lookup.setAutoDelete(TRUE);
  int loop;
  for (loop = 0; loop < bar->count(); loop++)
  {
    Bar *r = new Bar;
    QDateTime dt;
    bar->getDate(loop, dt);
    r->setDate(dt);
    r->setClose(bar->getClose(loop));
    r->getDateTimeString(FALSE, s);
    lookup.insert(s, r);
  }

  // match all second symbol bars
  for (loop = bar2->count() - 1; loop > -1; loop--)
  {
    Bar r;
    QDateTime dt;
    bar2->getDate(loop, dt);
    s = dt.toString("yyyyMMddhhmmss");
    Bar *tr = lookup.find(s);
    if (tr)
    {
      double t = tr->getClose() - bar2->getClose(loop);
      r.setDate(dt);
      r.setOpen(t);
      r.setHigh(t);
      r.setLow(t);
      r.setClose(t);
      barData->prepend(r);
    }
  }

  delete bar;
  delete bar2;
}

bool Spread::createNew (QString &path, QString &symbol)
{
  bool ok = FALSE;
  symbol = QInputDialog::getText(QObject::tr("New Spread"),
                                 QObject::tr("Enter symbol name for the new Spread"),
                                 QLineEdit::Normal,
                                 QString::null,
                                 &ok,
                                 0);
  if (! symbol.length() || ok == FALSE)
    return FALSE;

  QDir dir;
  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/Spread");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
                               QObject::tr("Could not create Spread directory."));
      return FALSE;
    }
  }

  s.append("/" + symbol);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
                             QObject::tr("This Spread already exists."));
    return FALSE;
  }

  path = s;
  return TRUE;
}

bool Spread::prefDialog (QString &fs, QString &ss)
{
  QString pl = QObject::tr("Parms");
  QString fsl = QObject::tr("First Symbol");
  QString ssl = QObject::tr("Second Symbol");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Edit Spread"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  dialog->addSymbolItem(fsl, pl, s, fs);
  dialog->addSymbolItem(ssl, pl, s, ss);

  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
  {
    dialog->getSymbol(fsl, fs);
    dialog->getSymbol(ssl, ss);
    delete dialog;
    return TRUE;
  }
  delete dialog;
  return FALSE;
}
