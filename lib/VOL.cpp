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

#include "VOL.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>

VOL::VOL ()
{
  pluginName = "VOL";
  helpFile = "vol.html";

  methodList.append("VOL");
  methodList.append("NVI");
  methodList.append("PVI");
  methodList.append("PVT");
  methodList.sort();

  upColorLabel = "upColor";
  downColorLabel = "downColor";
  vtColorLabel = "vtColor";
  volLabelLabel = "volLabel";
  volLineTypeLabel = "volLineType";
  vtLineTypeLabel = "vtLineType";
  maColorLabel = "maColor";
  maPeriodLabel = "maPeriod";
  maLabelLabel = "maLabel";
  maLineTypeLabel = "maLineType";
  maTypeLabel = "maType";
  labelLabel = "label";
  methodLabel = "method";
  pluginLabel = "plugin";

  formatList.append(FormatString);

  setDefaults();
}

VOL::~VOL ()
{
}

void VOL::setDefaults ()
{
  upColor.setNamedColor("#009900");
  downColor.setNamedColor("#ee0000");
  maColor.setNamedColor("#ffff33");
  vtColor.setNamedColor("#ee0000");
  volLineType = PlotLine::HistogramBar;
  maLineType = PlotLine::Line;
  vtLineType = PlotLine::Line;
  volLabel = "VOL";
  label = volLabel;
  maLabel = "MAVol";
  period = 0;
  maType = 1;
  method = "VOL";
}

Indicator * VOL::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  PlotLine *line = 0;
  if (! method.compare("VOL"))
    line = calculateVOL();
  else
  {
    if (! method.compare("NVI"))
      line = calculateNVI();
    else
    {
      if (! method.compare("PVI"))
        line = calculatePVI();
      else
        line = calculatePVT();
    }
  }

  output->addLine(line);

  if (period < 1)
    return output;

  PlotLine *ma = getMA(line, maType, period);
  ma->setColor(maColor);
  ma->setType(maLineType);
  ma->setLabel(maLabel);
  output->addLine(ma);

  return output;
}

PlotLine * VOL::calculateVOL ()
{
  PlotLine *pl = data->getInput(BarData::Volume);
  pl->setType(volLineType);
  pl->setLabel(volLabel);
  pl->setColorFlag(TRUE);

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    if (loop > 0)
    {
      if (data->getClose(loop) > data->getClose(loop - 1))
        pl->setColorBar(loop, upColor);
      else
        pl->setColorBar(loop, downColor);
    }
    else
      pl->setColorBar(loop, upColor);
  }

  return pl;
}

PlotLine * VOL::calculateNVI ()
{
  PlotLine *nvi = new PlotLine();
  nvi->setColor(vtColor);
  nvi->setType(vtLineType);
  nvi->setLabel(label);

  int loop;
  double nv = 1000;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    double close = data->getClose(loop);
    double yvolume = data->getVolume(loop - 1);
    double yclose = data->getClose(loop - 1);

    if (volume < yvolume)
      nv = nv + ((close - yclose) / yclose) * nv;

    nvi->append(nv);
  }

  return nvi;
}

PlotLine * VOL::calculatePVI ()
{
  PlotLine *pvi = new PlotLine();
  pvi->setColor(vtColor);
  pvi->setType(vtLineType);
  pvi->setLabel(label);

  int loop = 0;
  double pv = 1000;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double volume = data->getVolume(loop);
    double close = data->getClose(loop);
    double yvolume = data->getVolume(loop - 1);
    double yclose = data->getClose(loop - 1);

    if (volume > yvolume)
      pv = pv + ((close - yclose) / yclose) * pv;

    pvi->append(pv);
  }

  return pvi;
}

PlotLine * VOL::calculatePVT ()
{
  PlotLine *pvt = new PlotLine();
  pvt->setColor(vtColor);
  pvt->setType(vtLineType);
  pvt->setLabel(label);

  int loop = 0;
  double pv = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double close = data->getClose(loop);
    double volume = data->getVolume(loop);
    double yclose = data->getClose(loop - 1);

    pv = pv + (((close - yclose) / yclose) * volume);
    pvt->append(pv);
  }

  return pvt;
}

int VOL::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("VOL");
  QString ml = QObject::tr("Method");
  QString ucl = QObject::tr("Up Color");
  QString dcl = QObject::tr("Down Color");
  QString cl = QObject::tr("Color");
  QString vll = QObject::tr("VOL Label");
  QString vltl = QObject::tr("VOL Line Type");
  QString ltl = QObject::tr("Line Type");
  QString pl2 = QObject::tr("MA");
  QString mcl = QObject::tr("MA Color");
  QString mpl = QObject::tr("MA Period");
  QString mll = QObject::tr("MA Label");
  QString mltl = QObject::tr("MA Line Type");
  QString mtl = QObject::tr("MA Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VOL Indicator"));
  dialog->setHelpFile(helpFile);

  dialog->createPage (pl);
  dialog->addComboItem(ml, pl, methodList, method);
  dialog->addColorItem(ucl, pl, upColor);
  dialog->addColorItem(dcl, pl, downColor);
  dialog->addColorItem(cl, pl, vtColor);
  dialog->addTextItem(vll, pl, volLabel);
  dialog->addComboItem(vltl, pl, lineTypes, volLineType);
  dialog->addComboItem(ltl, pl, lineTypes, vtLineType);

  dialog->createPage (pl2);
  dialog->addColorItem(mcl, pl2, maColor);
  dialog->addIntItem(mpl, pl2, period, 0, 99999999);
  dialog->addTextItem(mll, pl2, maLabel);
  dialog->addComboItem(mltl, pl2, lineTypes, maLineType);
  QStringList l;
  getMATypes(l);
  dialog->addComboItem(mtl, pl2, l, maType);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getCombo(ml, method);
    dialog->getColor(ucl, upColor);
    dialog->getColor(dcl, downColor);
    dialog->getColor(cl, vtColor);
    dialog->getText(vll, volLabel);
    label = volLabel;
    volLineType = (PlotLine::LineType) dialog->getComboIndex(vltl);
    vtLineType = (PlotLine::LineType) dialog->getComboIndex(ltl);

    dialog->getColor(mcl, maColor);
    period = dialog->getInt(mpl);
    dialog->getText(mll, maLabel);
    maLineType = (PlotLine::LineType) dialog->getComboIndex(mltl);
    maType = dialog->getComboIndex(mtl);

    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void VOL::setIndicatorSettings (Setting &dict)
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

  dict.getData(vtColorLabel, s);
  if (s.length())
    vtColor.setNamedColor(s);

  dict.getData(volLabelLabel, s);
  if (s.length())
    volLabel = s;

  dict.getData(volLineTypeLabel, s);
  if (s.length())
    volLineType = (PlotLine::LineType) s.toInt();

  dict.getData(vtLineTypeLabel, s);
  if (s.length())
    vtLineType = (PlotLine::LineType) s.toInt();

  dict.getData(maColorLabel, s);
  if (s.length())
    maColor.setNamedColor(s);

  dict.getData(maPeriodLabel, s);
  if (s.length())
    period = s.toInt();

  dict.getData(maLabelLabel, s);
  if (s.length())
    maLabel = s;

  dict.getData(maLineTypeLabel, s);
  if (s.length())
    maLineType = (PlotLine::LineType) s.toInt();

  dict.getData(maTypeLabel, s);
  if (s.length())
    maType = s.toInt();

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(methodLabel, s);
  if (s.length())
    method = s;
}

void VOL::getIndicatorSettings (Setting &dict)
{
  QString ts = upColor.name();
  dict.setData(upColorLabel, ts);
  ts = downColor.name();
  dict.setData(downColorLabel, ts);
  ts = vtColor.name();
  dict.setData(vtColorLabel, ts);
  dict.setData(volLabelLabel, volLabel);
  ts = QString::number(volLineType);
  dict.setData(volLineTypeLabel, ts);
  ts = QString::number(vtLineType);
  dict.setData(vtLineTypeLabel, ts);
  ts = maColor.name();
  dict.setData(maColorLabel, ts);
  ts = QString::number(period);
  dict.setData(maPeriodLabel, ts);
  dict.setData(maLabelLabel, maLabel);
  ts = QString::number(maLineType);
  dict.setData(maLineTypeLabel, ts);
  ts = QString::number(maType);
  dict.setData(maTypeLabel, ts);
  dict.setData(labelLabel, label);
  dict.setData(methodLabel, method);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * VOL::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: METHOD

  if (checkFormat(p, d, 1, 1))
    return 0;

  method = formatStringList[0];
  if (methodList.findIndex(method) == -1)
  {
    qDebug("VOL::calculateCustom: invalid METHOD parm");
    return 0;
  }

  PlotLine *line = 0;
  if (! method.compare("VOL"))
    line = calculateVOL();
  else
  {
    if (! method.compare("NVI"))
      line = calculateNVI();
    else
    {
      if (! method.compare("PVI"))
        line = calculatePVI();
      else
        line = calculatePVT();
    }
  }

  return line;
}

void VOL::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString ml = QObject::tr("Method");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("VOL Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  // format1: METHOD

  QString s;
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(ml, pl, methodList, method);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    dialog->getCombo(ml, rs);
  }

  delete dialog;
}
