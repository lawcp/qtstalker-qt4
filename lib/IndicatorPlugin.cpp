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

#include "IndicatorPlugin.h"
#include "TALIB.h"
#include <qfile.h>
#include <q3textstream.h>
//Added by qt3to4:
#include <Q3PtrList>

IndicatorPlugin::IndicatorPlugin()
{
  saveFlag = FALSE;
  dateFlag = FALSE;
  logScale = FALSE;

  PlotLine pl;
  pl.getLineTypes(lineTypes);

  BarData it(pluginName);
  it.getInputFields(inputTypeList);

  opList.append("EQ");
  opList.append("LT");
  opList.append("LTEQ");
  opList.append("GT");
  opList.append("GTEQ");
  opList.append("AND");
  opList.append("OR");
  opList.append("XOR");

  maList.append("SMA"); //    TA_MAType_SMA       =0,
  maList.append("EMA"); //    TA_MAType_EMA       =1,
  maList.append("WMA"); //    TA_MAType_WMA       =2,
  maList.append("DEMA"); //    TA_MAType_DEMA      =3,
  maList.append("TEMA"); //    TA_MAType_TEMA      =4,
  maList.append("TRIMA"); //    TA_MAType_TRIMA     =5,
  maList.append("KAMA"); //    TA_MAType_KAMA      =6,
  maList.append("MAMA"); //    TA_MAType_MAMA      =7,
  maList.append("T3"); //    TA_MAType_T3        =8
  maList.append("Wilder");
}

IndicatorPlugin::~IndicatorPlugin()
{
}

void IndicatorPlugin::setIndicatorInput (BarData *d)
{
  data = d;
}

void IndicatorPlugin::loadFile (QString &file, Setting &dict)
{
  QFile f(file);
  if (! f.open(QIODevice::ReadOnly))
  {
    qDebug("IndicatorPlugin:can't read file %s", file.latin1());
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

    if (l.count() > 2)
    {
      QString k = l[0];
      s = s.remove(0, k.length() + 1);
      dict.setData(k, s);
    }
    else
      dict.setData(l[0], l[1]);
  }

  f.close();
}

void IndicatorPlugin::saveFile (QString &file, Setting &dict)
{
  QFile f(file);
  if (! f.open(QIODevice::WriteOnly))
  {
    qDebug("IndicatorPlugin:can't save file %s", file.latin1());
    return;
  }
  Q3TextStream stream(&f);

  QStringList key;
  dict.getKeyList(key);

  int loop;
  QString s;
  for(loop = 0; loop < (int) key.count(); loop++)
  {
    dict.getData(key[loop], s);
    stream << key[loop] << "=" << s << "\n";
  }

  f.close();
}

void IndicatorPlugin::getMATypes (QStringList &l)
{
  l = maList;
}

PlotLine * IndicatorPlugin::getMA (PlotLine *in, int type, int period)
{
  PlotLine *ma = 0;
  TALIB plug;
  if (type == 9)
    ma = getWilderMA(in, period);
  else
    ma = plug.getMA(in, type, period);
  return ma;
}

PlotLine * IndicatorPlugin::getWilderMA (PlotLine *d, int period)
{
  PlotLine *wilderma = new PlotLine;

  if (period >= (int) d->getSize())
    return wilderma;

  if (period < 1)
    return wilderma;

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + d->getData(loop);

  double yesterday = t / period;

  wilderma->append(yesterday);

  for (; loop < (int) d->getSize(); loop++)
  {
    double t  = (yesterday * (period - 1) + d->getData(loop))/period;
    yesterday = t;
    wilderma->append(t);
  }

  return wilderma;
}

void IndicatorPlugin::getPluginName (QString &d)
{
  d = pluginName;
}

void IndicatorPlugin::getHelpFile (QString &d)
{
  d = helpFile;
}

IndicatorPlugin::Operator IndicatorPlugin::getOperator (QString &d)
{
  int i = opList.findIndex(d);
  return (Operator) i;
}

bool IndicatorPlugin::checkFormat (QString &p, Q3PtrList<PlotLine> &d, int hrange, int lrange)
{
  formatStringList = QStringList::split(",", p, FALSE);

  if ((int) formatStringList.count() < lrange || (int) formatStringList.count() > hrange)
  {
    qDebug("%s::checkFormat: invalid parm count", pluginName.latin1());
    return TRUE;
  }

  int loop;
  for (loop = 0; loop < (int) formatList.count(); loop++)
  {
    if (formatList[loop] == FormatInputArray)
    {
      if (! d.count())
      {
        qDebug("%s::checkFormat: parm #%i invalid, no INPUT_ARRAY found", pluginName.latin1(), loop+1);
        return TRUE;
      }
      continue;
    }

    if (formatList[loop] == FormatInputArray2)
    {
      if (d.count() != 2)
      {
        qDebug("%s::checkFormat: parm #%i invalid, no INPUT_ARRAY2 found", pluginName.latin1(), loop+1);
        return TRUE;
      }
      continue;
    }

    if (formatList[loop] == FormatInteger)
    {
      bool ok;
      formatStringList[loop].toInt(&ok);
      if (! ok)
      {
        qDebug("%s::checkFormat: parm #%i invalid, not an INTEGER", pluginName.latin1(), loop + 1);
        return TRUE;
      }
      continue;
    }

    if (formatList[loop] == FormatDouble)
    {
      bool ok;
      formatStringList[loop].toDouble(&ok);
      if (! ok)
      {
        qDebug("%s::checkFormat: parm #%i invalid, not a DOUBLE", pluginName.latin1(), loop + 1);
        return TRUE;
      }
      continue;
    }

    if (formatList[loop] == FormatMAType)
    {
      QStringList mal;
      getMATypes(mal);
      if (mal.findIndex(formatStringList[loop]) == -1)
      {
        qDebug("%s::checkFormat: parm #%i invalid, not an MA_TYPE", pluginName.latin1(), loop + 1);
        return TRUE;
      }
      continue;
    }

    if (formatList[loop] == FormatBool)
    {
      if (! formatStringList[loop].compare("TRUE"))
        continue;
      else
      {
        if (! formatStringList[loop].compare("FALSE"))
          continue;
        else
        {
          qDebug("%s::checkFormat: parm #%i invalid, not a BOOL", pluginName.latin1(), loop + 1);
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

void IndicatorPlugin::wakeup ()
{
  emit signalWakeup();
}

void IndicatorPlugin::setFormatMethod (QString &d)
{
  formatMethod = d;
}

//***************************************************************
//****************** VIRTUAL OVERIDES ***************************
//***************************************************************

Indicator * IndicatorPlugin::calculate ()
{
  return 0;
}

int IndicatorPlugin::indicatorPrefDialog (QWidget *)
{
  return 0;
}

PlotLine * IndicatorPlugin::calculateCustom (QString &, Q3PtrList<PlotLine> &)
{
  return 0;
}

void IndicatorPlugin::getIndicatorSettings (Setting &)
{
}

void IndicatorPlugin::setIndicatorSettings (Setting &)
{
}

void IndicatorPlugin::setCustomFunction (QStringList &)
{
}

void IndicatorPlugin::saveIndicatorSettings (QString &d)
{
  Setting set;
  getIndicatorSettings(set);
  saveFile(d, set);
}

void IndicatorPlugin::loadIndicatorSettings (QString &d)
{
  Setting set;
  loadFile(d, set);
  QString k = "dateFlag";
  dateFlag = set.getInt(k);
  k = "logScale";
  logScale = set.getInt(k);
  setIndicatorSettings(set);
}

void IndicatorPlugin::formatDialog (QStringList &, QString &, QString &)
{
}
