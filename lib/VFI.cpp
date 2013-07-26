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

#include "VFI.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <math.h>

VFI::VFI ()
{
  pluginName = "VFI";
  helpFile = "vfi.html";

  colorLabel = "color";
  lineTypeLabel = "lineType";
  labelLabel = "label";
  periodLabel = "period";
  smoothingLabel = "smoothing";
  maTypeLabel = "maType";
  pluginLabel = "plugin";

  formatList.append(FormatMAType);
  formatList.append(FormatInteger);
  formatList.append(FormatInteger);

  setDefaults();
}

VFI::~VFI ()
{
}

void VFI::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 3;
  maType = 1;
  period = 100;
}

Indicator * VFI::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);
  output->addLine(getVFI());
  return output;
}

PlotLine * VFI::getVFI ()
{
  PlotLine *vfi = new PlotLine();
  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = data->getClose(loop-period);
    double high = data->getHigh(loop-period);
    double low = data->getLow(loop-period);
    double typical = (high+low+close)/3.0;
    for(i=loop-period+1; i<=loop; i++) {
      double ytypical = typical;
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high+low+close)/3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta*delta;
      sma_vol += data->getVolume(i);
      }
    inter = 0.2*sqrt(inter/(double)period)*typical;
    sma_vol /= (double)period;

    close = data->getClose(loop-period);
    high = data->getHigh(loop-period);
    low = data->getLow(loop-period);
    typical = (high+low+close)/3.0;
    double t = 0;
    for(i=loop-period+1; i<=loop; i++) {
      double ytypical = typical;
      double volume = data->getVolume(i);
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high+low+close)/3.0;

      if (typical > ytypical+inter)
        t = t + log(1.0 + volume/sma_vol);
      else
      {
        if (typical < ytypical-inter)
          t = t - log(1.0 + volume/sma_vol);
      }
    }

    vfi->append(t);
  }

  PlotLine *pl = 0;
  if (smoothing > 1)
  {
    PlotLine *ma = getMA(vfi, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    pl = ma;
    delete vfi;
  }
  else
  {
    vfi->setColor(color);
    vfi->setType(lineType);
    vfi->setLabel(label);
    pl = vfi;
  }

  return pl;
}

int VFI::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VFI Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  QStringList l;
  getMATypes(l);
  dialog->addComboItem(stl, pl, l, maType);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    dialog->getText(ll, label);
    period = dialog->getInt(perl);
    smoothing = dialog->getInt(sl);
    maType = dialog->getComboIndex(stl);
    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void VFI::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getData(colorLabel, s);
  if (s.length())
    color.setNamedColor(s);

  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(periodLabel, s);
  if (s.length())
    period = s.toInt();

  dict.getData(smoothingLabel, s);
  if (s.length())
    smoothing = s.toInt();

  dict.getData(maTypeLabel, s);
  if (s.length())
    maType = s.toInt();

}

void VFI::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(period);
  dict.setData(periodLabel, ts);
  ts = QString::number(smoothing);
  dict.setData(smoothingLabel, ts);
  ts = QString::number(maType);
  dict.setData(maTypeLabel, ts);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * VFI::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: MA_TYPE, PERIOD, SMOOTHING

  if (checkFormat(p, d, 3, 3))
    return 0;

  QStringList mal;
  getMATypes(mal);
  maType = mal.findIndex(formatStringList[0]);
  period = formatStringList[1].toInt();
  smoothing = formatStringList[2].toInt();

  return getVFI();
}

void VFI::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("VFI Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  // format1: MA_TYPE, PERIOD, SMOOTHING

  QString s;
  QStringList l;
  getMATypes(l);
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(stl, pl, l, maType);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);

    dialog->getCombo(stl, rs);

    int t = dialog->getInt(perl);
    rs.append("," + QString::number(t));

    t = dialog->getInt(sl);
    rs.append("," + QString::number(t));
  }

  delete dialog;
}
