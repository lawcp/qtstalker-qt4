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

/* The "SafeZone Stop" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173 */

#include "SZ.h"
#include <math.h>
#include <stdio.h>
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>

SZ::SZ ()
{
  pluginName = "SZ";

  colorLabel = "color";
  lineTypeLabel = "lineType";
  periodLabel = "period";
  noDeclinePeriodLabel = "noDeclinePeriod";
  coefficientLabel = "coefficient";
  methodLabel = "method";
  labelLabel = "label";
  pluginLabel = "plugin";

  // format1: METHOD, PERIOD, NO_DECLINE_PERIOD, COEFFICIENT
  formatList.append(FormatString);
  formatList.append(FormatInteger);
  formatList.append(FormatInteger);
  formatList.append(FormatDouble);

  setDefaults();
  methodList.append("Long");
  methodList.append("Short");
  helpFile = "sz.html";
}

SZ::~SZ ()
{
}

void SZ::setDefaults ()
{
  color.setNamedColor("white");
  lineType = PlotLine::Line;
  coefficient = 2.5;
  period = 10;
  no_decline_period = 2;
  method = "Long";
  label = pluginName;
}

Indicator * SZ::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);
  output->addLine(getSZ());
  return output;
}

PlotLine * SZ::getSZ ()
{
  if (period < 1)
    period = 1;

  int display_uptrend = 0;
  int display_dntrend = 0;
  int position = 1;
  if (! method.compare("Long"))
    position = 1;
  else
    position = 2;
  if (position & 1) // long
    display_uptrend = 1;
  if (position & 2) // short
    display_dntrend = 1;

  PlotLine *sz_uptrend = new PlotLine();
  PlotLine *sz_dntrend = new PlotLine();

  double uptrend_stop = 0;
  double dntrend_stop = 0;

  if (no_decline_period < 0)
    no_decline_period = 0;
  if (no_decline_period > 365)
    no_decline_period = 365;

  double old_uptrend_stops[no_decline_period];
  double old_dntrend_stops[no_decline_period];

  int loop;
  for (loop = 0; loop < no_decline_period; loop++)
  {
    old_uptrend_stops[loop] = 0;
    old_dntrend_stops[loop] = 0;
  }

  int start = period + 1;
  for (loop = start; loop < (int) data->count(); loop++)
  {
    // calculate downside/upside penetration for lookback period
    int lbloop;
    int lbstart = loop - period;
    if (lbstart < 2)
      lbstart = 2;
    double uptrend_noise_avg = 0;
    double uptrend_noise_cnt = 0;
    double dntrend_noise_avg = 0;
    double dntrend_noise_cnt = 0;
    for (lbloop = lbstart; lbloop < loop; lbloop++)
    {
      double lo_curr = data->getLow(lbloop);
      double lo_last = data->getLow(lbloop - 1);
      double hi_curr = data->getHigh(lbloop);
      double hi_last = data->getHigh(lbloop - 1);
      if (lo_last > lo_curr)
      {
	uptrend_noise_avg += lo_last - lo_curr;
	uptrend_noise_cnt++;
      }
      if (hi_last < hi_curr)
      {
	dntrend_noise_avg += hi_curr - hi_last;
	dntrend_noise_cnt++;
      }
    }
    // make *_avg into actual averages
    if (uptrend_noise_cnt > 0)
      uptrend_noise_avg /= uptrend_noise_cnt;
    if (dntrend_noise_cnt > 0)
      dntrend_noise_avg /= dntrend_noise_cnt;

    double lo_last = data->getLow(loop - 1);
    double hi_last = data->getHigh(loop - 1);
    uptrend_stop = lo_last - coefficient * uptrend_noise_avg;
    dntrend_stop = hi_last + coefficient * dntrend_noise_avg;

    double adjusted_uptrend_stop = uptrend_stop;
    double adjusted_dntrend_stop = dntrend_stop;

    int backloop;
    for (backloop = no_decline_period - 1; backloop >= 0; backloop--)
    {
      if (loop - backloop > start)
      {
	if (old_uptrend_stops[backloop] > adjusted_uptrend_stop)
	  adjusted_uptrend_stop = old_uptrend_stops[backloop];
	if (old_dntrend_stops[backloop] < adjusted_dntrend_stop)
	  adjusted_dntrend_stop = old_dntrend_stops[backloop];
      }
      if (backloop > 0)
      {
	old_uptrend_stops[backloop] = old_uptrend_stops[backloop-1];
	old_dntrend_stops[backloop] = old_dntrend_stops[backloop-1];
      }
    }

    old_uptrend_stops[0] = uptrend_stop;
    old_dntrend_stops[0] = dntrend_stop;

    sz_uptrend->append(adjusted_uptrend_stop);
    sz_dntrend->append(adjusted_dntrend_stop);
  }

  PlotLine *pl = 0;
  if (display_uptrend)
  {
    sz_uptrend->setColor(color);
    sz_uptrend->setType(lineType);
    QString t = QObject::tr("SZ LONG");
    sz_uptrend->setLabel(t);
    pl = sz_uptrend;
  }

  if (display_dntrend)
  {
    sz_dntrend->setColor(color);
    sz_dntrend->setType(lineType);
    QString t = QObject::tr("SZ SHORT");
    sz_dntrend->setLabel(t);
    pl = sz_dntrend;
  }

  return pl;
}

int SZ::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString pos = QObject::tr("Position");
  QString lp = QObject::tr("Lookback Period");
  QString ndp = QObject::tr("No Decline Period");
  QString co = QObject::tr("Coefficient");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SZ Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addComboItem(pos, pl, methodList, method);
  dialog->addIntItem(lp, pl, period, 1, 99999999);
  dialog->addIntItem(ndp, pl, no_decline_period, 1, 99999999);
  dialog->addDoubleItem(co, pl, coefficient, 0, 99999999);
  dialog->addTextItem(ll, pl, label);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(lp);
    no_decline_period = dialog->getInt(ndp);
    coefficient = dialog->getDouble(co);
    dialog->getCombo(pos, method);
    dialog->getText(ll, label);
    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void SZ::setIndicatorSettings (Setting &dict)
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

  dict.getData(periodLabel, s);
  if (s.length())
    period = s.toInt();

  dict.getData(noDeclinePeriodLabel, s);
  if (s.length())
    no_decline_period = s.toInt();

  dict.getData(coefficientLabel, s);
  if (s.length())
    coefficient = s.toFloat();

  dict.getData(methodLabel, s);
  if (s.length())
    method = s;

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;
}

void SZ::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  ts = QString::number(period);
  dict.setData(periodLabel, ts);
  ts = QString::number(no_decline_period);
  dict.setData(noDeclinePeriodLabel, ts);
  ts = QString::number(coefficient);
  dict.setData(coefficientLabel, ts);
  dict.setData(methodLabel, method);
  dict.setData(labelLabel, label);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * SZ::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: METHOD, PERIOD, NO_DECLINE_PERIOD, COEFFICIENT

  if (checkFormat(p, d, 4, 4))
    return 0;

  if (methodList.findIndex(formatStringList[0]) == -1)
  {
    qDebug("SZ::calculateCustom: invalid METHOD parm");
    return 0;
  }

  method = formatStringList[0];
  period = formatStringList[1].toInt();
  no_decline_period = formatStringList[2].toInt();
  coefficient = formatStringList[3].toDouble();

  return getSZ();
}

void SZ::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString pos = QObject::tr("Position");
  QString lp = QObject::tr("Lookback Period");
  QString ndp = QObject::tr("No Decline Period");
  QString co = QObject::tr("Coefficient");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("SZ Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(pos, pl, methodList, method);
  dialog->addIntItem(lp, pl, period, 1, 99999999);
  dialog->addIntItem(ndp, pl, no_decline_period, 1, 99999999);
  dialog->addDoubleItem(co, pl, coefficient, 0, 99999999);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);

    dialog->getCombo(pos, rs);

    int t = dialog->getInt(lp);
    rs.append("," + QString::number(t));

    t = dialog->getInt(ndp);
    rs.append("," + QString::number(t));

    double d = dialog->getDouble(co);
    rs.append("," + QString::number(d));
  }

  delete dialog;
}
