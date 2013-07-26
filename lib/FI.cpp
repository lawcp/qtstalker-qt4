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

#include "FI.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>

FI::FI ()
{
  pluginName = "FI";
  helpFile = "fi.html";

  colorLabel = "color";
  lineTypeLabel = "lineType";
  smoothingLabel = "smoothing";
  labelLabel = "label";
  maTypeLabel = "maType";
  pluginLabel = "plugin";

  formatList.append(FormatMAType);
  formatList.append(FormatInteger);

  setDefaults();
}

FI::~FI ()
{
}

void FI::setDefaults ()
{
  color.setNamedColor("orange");
  lineType = PlotLine::HistogramBar;
  label = pluginName;
  smoothing = 2;
  maType = 0;
}

Indicator * FI::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  PlotLine *fi = getFI();
  if (smoothing > 1)
  {
    PlotLine *ma = getMA(fi, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete fi;
  }
  else
  {
    fi->setColor(color);
    fi->setType(lineType);
    fi->setLabel(label);
    output->addLine(fi);
  }
  return output;
}

PlotLine * FI::getFI ()
{
  PlotLine *fi = new PlotLine();
  int loop;
  double force = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double cdiff = data->getClose(loop) - data->getClose(loop - 1);
    force = data->getVolume(loop) * cdiff;
    fi->append(force);
  }

  return fi;
}

int FI::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("FI Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  QStringList l;
  getMATypes(l);
  dialog->addComboItem(stl, pl, l, maType);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    smoothing = dialog->getInt(sl);
    dialog->getText(ll, label);
    maType = dialog->getComboIndex(stl);
    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void FI::setIndicatorSettings (Setting &dict)
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

  dict.getData(smoothingLabel, s);
  if (s.length())
    smoothing = s.toInt();

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(maTypeLabel, s);
  if (s.length())
    maType = s.toInt();
}

void FI::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  ts = QString::number(smoothing);
  dict.setData(smoothingLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(maType);
  dict.setData(maTypeLabel, ts);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * FI::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: MA_TYPE, SMOOTHING

  if (checkFormat(p, d, 2, 2))
    return 0;

  QStringList mal;
  getMATypes(mal);
  maType = mal.findIndex(formatStringList[0]);
  smoothing = formatStringList[1].toInt();

  PlotLine *fi = getFI();
  PlotLine *ma = getMA(fi, maType, smoothing);
  delete fi;

  return ma;
}

void FI::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vl = QObject::tr("Variable Name");
  QString mtl = QObject::tr("MA Type");
  QString ppl = QObject::tr("Period");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("FI Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  QStringList mal;
  getMATypes(mal);
  dialog->addTextItem(vl, pl, s);
  dialog->addComboItem(mtl, pl, mal, maType);
  dialog->addIntItem(ppl, pl, smoothing, 1, 999999);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vl, rv);
    dialog->getCombo(mtl, rs);
    int t = dialog->getInt(ppl);
    rs.append("," + QString::number(t));
  }

  delete dialog;
}
