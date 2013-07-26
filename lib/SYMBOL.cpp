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

#include "SYMBOL.h"
#include "DbPlugin.h"
#include "Config.h"
#include "DBIndex.h"
#include "PrefDialog.h"
#include <qdatetime.h>
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>

SYMBOL::SYMBOL ()
{
  pluginName = "SYMBOL";
  formatList.append(FormatString);
  helpFile = "symbol.html";
}

SYMBOL::~SYMBOL ()
{
}

PlotLine * SYMBOL::getSYMBOL ()
{
  QString s;
  Config config;
  config.getData(Config::IndexPath, s);
  DBIndex index;
  index.open(s);

  PlotLine *line = new PlotLine();

  DbPlugin db;
  if (db.open(symbol, &index))
  {
    db.close();
    index.close();
    return line;
  }

  QDateTime date;
  data->getDate(0, date);

  QString ts;
  config.getData(Config::BarLength, ts);
  db.setBarLength((BarData::BarLength) ts.toInt());
  config.getData(Config::Bars, ts);
  db.setBarRange(ts.toInt());
  BarData *recordList = new BarData(symbol);
  QDateTime dt = QDateTime::currentDateTime();
  db.getHistory(recordList, dt);

  Q3Dict<Setting> dict;
  dict.setAutoDelete(TRUE);

  int loop;
  ts = "Close";
  QString ts2;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = new Setting;
    ts2 = QString::number(recordList->getClose(loop));
    r->setData(ts, ts2);
    recordList->getDate(loop, dt);
    QString s = dt.toString("yyyyMMddhhmmss");
    dict.insert(s, r);
  }

  double val = 0;

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    data->getDate(loop, dt);
    QString s = dt.toString("yyyyMMddhhmmss");
    Setting *r2 = dict[s];
    if (r2)
    {
      val = r2->getDouble(ts);
      line->append(val);
    }
  }

  delete recordList;
  db.close();
  index.close();

  line->setScaleFlag(TRUE);
  return line;
}

PlotLine * SYMBOL::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: SYMBOL

  if (checkFormat(p, d, 1, 1))
    return 0;

  symbol = formatStringList[0];

  return getSYMBOL();
}

void SYMBOL::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString sl = QObject::tr("Symbol");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("SYMBOL Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  Config config;
  dialog->addTextItem(vnl, pl, s);
  config.getData(Config::DataPath, s);
  dialog->addSymbolItem(sl, pl, s, symbol);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    dialog->getSymbol(sl, rs);
  }

  delete dialog;
}
