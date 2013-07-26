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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

#include "THERM.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <math.h>

THERM::THERM ()
{
  pluginName = "THERM";
  helpFile = "therm.html";

  upColorLabel = "upColor";
  downColorLabel = "downColor";
  threshColorLabel = "threshColor";
  labelLabel = "label";
  thresholdLabel = "threshold";
  smoothingLabel = "smoothing";
  smoothTypeLabel = "smoothType";
  maColorLabel = "maColor";
  maLineTypeLabel = "maLineType";
  maPeriodLabel = "maPeriod";
  maLabelLabel = "maLabel";
  maTypeLabel = "maType";
  pluginLabel = "plugin";

  // format1: MA_TYPE, MA_PERIOD, THRESHOLD, SMOOTHING_TYPE, SMOOTHING_PERIOD
  formatList.append(FormatMAType);
  formatList.append(FormatInteger);
  formatList.append(FormatDouble);
  formatList.append(FormatMAType);
  formatList.append(FormatInteger);

  setDefaults();
}

THERM::~THERM ()
{
}

void THERM::setDefaults ()
{
  downColor.setNamedColor("green");
  upColor.setNamedColor("magenta");
  threshColor.setNamedColor("red");
  maColor.setNamedColor("yellow");
  lineType = PlotLine::HistogramBar;
  maLineType = PlotLine::Line;
  label = pluginName;
  maLabel = QObject::tr("THERM MA");
  threshold = 3;
  smoothing = 2;
  maPeriod = 22;
  maType = 0;
  smoothType = 0;
}

Indicator * THERM::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getTHERM(pll);

  int loop;
  for (loop = 0; loop < (int) pll.count(); loop++)
    output->addLine(pll.at(loop));

  return output;
}

void THERM::getTHERM (Q3PtrList<PlotLine> &pll)
{
  PlotLine *therm = new PlotLine();
  int loop;
  double thermometer = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = fabs(data->getHigh(loop) - data->getHigh(loop - 1));
    double lo = fabs(data->getLow(loop - 1) - data->getLow(loop));

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    therm->append(thermometer);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(therm, smoothType, smoothing);
    pll.append(ma);
    delete therm;
    therm = ma;
  }
  else
    pll.append(therm);

  PlotLine *therm_ma = getMA(therm, maType, maPeriod);
  therm_ma->setColor(maColor);
  therm_ma->setType(maLineType);
  therm_ma->setLabel(maLabel);
  pll.append(therm_ma);

  // assign the therm colors

  therm->setColorFlag(TRUE);
  therm->setType(lineType);
  therm->setLabel(label);

  int thermLoop = therm->getSize() - 1;
  int maLoop = therm_ma->getSize() - 1;
  while (thermLoop > -1)
  {
    if (maLoop > -1)
    {
      double thrm = therm->getData(thermLoop);
      double thrmma = therm_ma->getData(maLoop);

      if (thrm > (thrmma * threshold))
        therm->setColorBar(thermLoop, threshColor);
      else
      {
        if (thrm > thrmma)
          therm->setColorBar(thermLoop, upColor);
        else
          therm->setColorBar(thermLoop, downColor);
      }
    }
    else
      therm->setColorBar(thermLoop, downColor);

    thermLoop--;
    maLoop--;
  }
}

int THERM::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("THERM Parms");
  QString pl2 = QObject::tr("MA Parms");
  QString cal = QObject::tr("Color Above MA");
  QString cbl = QObject::tr("Color Below MA");
  QString ctl = QObject::tr("Color Threshold");
  QString ll = QObject::tr("Label");
  QString tl = QObject::tr("Threshold");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  QString mcl = QObject::tr("MA Color");
  QString mltl = QObject::tr("MA Line Type");
  QString mll = QObject::tr("MA Label");
  QString mpl = QObject::tr("MA Period");
  QString mtl = QObject::tr("MA Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("THERM Indicator"));
  dialog->setHelpFile(helpFile);

  dialog->createPage (pl);
  dialog->addColorItem(cal, pl, upColor);
  dialog->addColorItem(cbl, pl, downColor);
  dialog->addColorItem(ctl, pl, threshColor);
  dialog->addTextItem(ll, pl, label);
  dialog->addDoubleItem(tl, pl, threshold, 1, 99999999);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  QStringList l;
  getMATypes(l);
  dialog->addComboItem(stl, pl, l, smoothType);

  dialog->createPage (pl2);
  dialog->addColorItem(mcl, pl2, maColor);
  dialog->addComboItem(mltl, pl2, lineTypes, maLineType);
  dialog->addTextItem(mll, pl2, maLabel);
  dialog->addIntItem(mpl, pl2, maPeriod, 0, 99999999);
  dialog->addComboItem(mtl, pl2, l, maType);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cal, upColor);
    dialog->getColor(cbl, downColor);
    dialog->getColor(ctl, threshColor);
    dialog->getText(ll, label);
    threshold = dialog->getDouble(tl);
    smoothing = dialog->getInt(sl);
    smoothType = dialog->getComboIndex(stl);

    dialog->getColor(mcl, maColor);
    maLineType = (PlotLine::LineType) dialog->getComboIndex(mltl);
    dialog->getText(mll, maLabel);
    maPeriod = dialog->getInt(mpl);
    maType = dialog->getComboIndex(mtl);
    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void THERM::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getData(upColorLabel, s);
  if (s.length())
    upColor.setNamedColor(s);

  dict.getData(downColorLabel, s);
  if (s.length())
    downColor.setNamedColor(s);

  dict.getData(threshColorLabel, s);
  if (s.length())
    threshColor.setNamedColor(s);

  dict.getData(maColorLabel, s);
  if (s.length())
    maColor.setNamedColor(s);

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(thresholdLabel, s);
  if (s.length())
    threshold = s.toFloat();

  dict.getData(smoothingLabel, s);
  if (s.length())
    smoothing = s.toInt();

  dict.getData(smoothTypeLabel, s);
  if (s.length())
    smoothType = s.toInt();

  dict.getData(maLineTypeLabel, s);
  if (s.length())
    maLineType = (PlotLine::LineType) s.toInt();

  dict.getData(maLabelLabel, s);
  if (s.length())
    maLabel = s;

  dict.getData(maPeriodLabel, s);
  if (s.length())
    maPeriod = s.toInt();

  dict.getData(maTypeLabel, s);
  if (s.length())
    maType = s.toInt();
}

void THERM::getIndicatorSettings (Setting &dict)
{
  QString ts = upColor.name();
  dict.setData(upColorLabel, ts);
  ts = downColor.name();
  dict.setData(downColorLabel, ts);
  ts = threshColor.name();
  dict.setData(threshColorLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(threshold);
  dict.setData(thresholdLabel, ts);
  ts = QString::number(smoothing);
  dict.setData(smoothingLabel, ts);
  ts = QString::number(smoothType);
  dict.setData(smoothTypeLabel, ts);

  ts = maColor.name();
  dict.setData(maColorLabel, ts);
  ts = QString::number(maLineType);
  dict.setData(maLineTypeLabel, ts);
  ts = QString::number(maPeriod);
  dict.setData(maPeriodLabel, ts);
  dict.setData(maLabelLabel, maLabel);
  ts = QString::number(maType);
  dict.setData(maTypeLabel, ts);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * THERM::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: MA_TYPE, MA_PERIOD, THRESHOLD, SMOOTHING_TYPE, SMOOTHING_PERIOD

  if (checkFormat(p, d, 5, 5))
    return 0;

  QStringList mal;
  getMATypes(mal);
  maType = mal.findIndex(formatStringList[0]);
  maPeriod = formatStringList[1].toInt();
  threshold = formatStringList[2].toDouble();
  smoothType = mal.findIndex(formatStringList[3]);
  smoothing = formatStringList[4].toInt();

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getTHERM(pll);

  int loop;
  for (loop = pll.count() - 1; loop > 0; loop--)
    pll.remove(loop);

  return pll.at(0);
}

void THERM::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString tl = QObject::tr("Threshold");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  QString mpl = QObject::tr("MA Period");
  QString mtl = QObject::tr("MA Type");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("THERM Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  QStringList l;
  getMATypes(l);
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(mtl, pl, l, maType);
  dialog->addIntItem(mpl, pl, maPeriod, 0, 99999999);
  dialog->addDoubleItem(tl, pl, threshold, 1, 99999999);
  dialog->addComboItem(stl, pl, l, smoothType);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);

    dialog->getCombo(mtl, rs);

    int t = dialog->getInt(mpl);
    rs.append("," + QString::number(t));

    double d = dialog->getDouble(tl);
    rs.append("," + QString::number(d));

    dialog->getCombo(stl, s);
    rs.append("," + s);

    t = dialog->getInt(sl);
    rs.append("," + QString::number(t));
  }

  delete dialog;
}

/* Alert Notes

1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days

*/
