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

#include "PP.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>

PP::PP ()
{
  pluginName = "PP";
  helpFile = "pp.html";

  ppList.append("FR");
  ppList.append("SR");
  ppList.append("TR");
  ppList.append("FS");
  ppList.append("SS");
  ppList.append("TS");

  resColorLabel = "resColor";
  supColorLabel = "supColor";
  resLineTypeLabel = "resLineType";
  supLineTypeLabel = "supLineType";
  resLabelLabel = "resLabel";
  resLabel2Label = "resLabel2";
  resLabel3Label = "resLabel3";
  supLabelLabel = "supLabel";
  supLabel2Label = "supLabel2";
  supLabel3Label = "supLabel3";
  labelLabel = "label";
  pluginLabel = "plugin";

  formatList.append(FormatString);

  setDefaults();
}

PP::~PP ()
{
}

void PP::setDefaults ()
{
  resColor.setNamedColor("yellow");
  supColor.setNamedColor("red");
  resLineType = PlotLine::Horizontal;
  supLineType = PlotLine::Horizontal;
  supLabel = QObject::tr("PP FS");
  supLabel2 = QObject::tr("PP SS");
  supLabel3 = QObject::tr("PP TS");
  resLabel = QObject::tr("PP FR");
  resLabel2 = QObject::tr("PP SR");
  resLabel3 = QObject::tr("PP TR");
  label = pluginName;
}

Indicator * PP::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getPP(pll);

  int loop;
  for (loop = 0; loop < (int) pll.count(); loop++)
    output->addLine(pll.at(loop));

  return output;
}

void PP::getPP (Q3PtrList<PlotLine> &pll)
{
  double high = data->getHigh(data->count() - 1);
  double low = data->getLow(data->count() - 1);
  double close = data->getClose(data->count() - 1);

  PlotLine *fr = new PlotLine();
  fr->setColor(resColor);
  fr->setType(resLineType);
  fr->setLabel(resLabel);
  double pp = (high + low + close) / 3;
  double t = (2 * pp) - low;
  fr->append(t);

  PlotLine *sr = new PlotLine();
  sr->setColor(resColor);
  sr->setType(resLineType);
  sr->setLabel(resLabel2);
  pp = (high + low + close) / 3;
  t = pp + (high - low);
  sr->append(t);

  PlotLine *thr = new PlotLine();
  thr->setColor(resColor);
  thr->setType(resLineType);
  thr->setLabel(resLabel3);
  pp = (high + low + close) / 3;
  t = (2 * pp) + (high - (2 * low));
  thr->append(t);

  PlotLine *fs = new PlotLine();
  fs->setColor(supColor);
  fs->setType(supLineType);
  fs->setLabel(supLabel);
  pp = (high + low + close) / 3;
  t = (2 * pp) - high;
  fs->append(t);

  PlotLine *ss = new PlotLine();
  ss->setColor(supColor);
  ss->setType(supLineType);
  ss->setLabel(supLabel2);
  pp = (high + low + close) / 3;
  t = pp - (high - low);
  ss->append(t);

  PlotLine *ts = new PlotLine();
  ts->setColor(supColor);
  ts->setType(supLineType);
  ts->setLabel(supLabel3);
  pp = (high + low + close) / 3;
  t = (2 * pp) - ((2 * high) - low);
  ts->append(t);

  pll.append(fr);
  pll.append(sr);
  pll.append(thr);
  pll.append(fs);
  pll.append(ss);
  pll.append(ts);
}

int PP::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Support");
  QString scl = QObject::tr("Support Color");
  QString lfsl = QObject::tr("Label First Support");
  QString lssl = QObject::tr("Label Second Support");
  QString ltsl = QObject::tr("Label Third Support");
  QString pl2 = QObject::tr("Resistance");
  QString rcl = QObject::tr("Resistance Color");
  QString lfrl = QObject::tr("Label First Resistance");
  QString lsrl = QObject::tr("Label Second Resistance");
  QString ltrl = QObject::tr("Label Third Resistance");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("PP Indicator"));
  dialog->setHelpFile(helpFile);

  dialog->createPage (pl);
  dialog->addColorItem(scl, pl, supColor);
  dialog->addTextItem(lfsl, pl, supLabel);
  dialog->addTextItem(lssl, pl, supLabel2);
  dialog->addTextItem(ltsl, pl, supLabel3);

  dialog->createPage (pl2);
  dialog->addColorItem(rcl, pl2, resColor);
  dialog->addTextItem(lfrl, pl2, resLabel);
  dialog->addTextItem(lsrl, pl2, resLabel2);
  dialog->addTextItem(ltrl, pl2, resLabel3);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(scl, supColor);
    dialog->getColor(rcl, resColor);
    dialog->getText(lfsl, supLabel);
    dialog->getText(lssl, supLabel2);
    dialog->getText(ltsl, supLabel3);
    dialog->getText(lfrl, resLabel);
    dialog->getText(lsrl, resLabel2);
    dialog->getText(ltrl, resLabel3);
    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void PP::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getData(resColorLabel, s);
  if (s.length())
    resColor.setNamedColor(s);

  dict.getData(supColorLabel, s);
  if (s.length())
    supColor.setNamedColor(s);

  dict.getData(resLineTypeLabel, s);
  if (s.length())
    resLineType = (PlotLine::LineType) s.toInt();

  dict.getData(supLineTypeLabel, s);
  if (s.length())
    supLineType = (PlotLine::LineType) s.toInt();

  dict.getData(resLabelLabel, s);
  if (s.length())
    resLabel = s;

  dict.getData(resLabel2Label, s);
  if (s.length())
    resLabel2 = s;

  dict.getData(resLabel3Label, s);
  if (s.length())
    resLabel3 = s;

  dict.getData(supLabelLabel, s);
  if (s.length())
    supLabel = s;

  dict.getData(supLabel2Label, s);
  if (s.length())
    supLabel2 = s;

  dict.getData(supLabel3Label, s);
  if (s.length())
    supLabel3 = s;

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;
}

void PP::getIndicatorSettings (Setting &dict)
{
  QString ts = resColor.name();
  dict.setData(resColorLabel, ts);
  ts = supColor.name();
  dict.setData(supColorLabel, ts);
  ts = QString::number(resLineType);
  dict.setData(resLineTypeLabel, ts);
  ts = QString::number(supLineType);
  dict.setData(supLineTypeLabel, ts);
  dict.setData(resLabelLabel, resLabel);
  dict.setData(resLabel2Label, resLabel2);
  dict.setData(resLabel3Label, resLabel3);
  dict.setData(supLabelLabel, supLabel);
  dict.setData(supLabel2Label, supLabel2);
  dict.setData(supLabel3Label, supLabel3);
  dict.setData(labelLabel, label);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * PP::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: PP_TYPE
  if (checkFormat(p, d, 1, 1))
    return 0;

  int t = ppList.findIndex(formatStringList[0]);
  if (t == -1)
  {
    qDebug("PP::calculateCustom: invalid PP_TYPE parm");
    return 0;
  }

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(TRUE);
  getPP(pll);

  PlotLine *line = new PlotLine;
  PlotLine *tline = pll.at(t);
  line->copy(tline);

  return line;
}

void PP::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString ppl = QObject::tr("PP Level");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("PP Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(ppl, pl, ppList, 0);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    dialog->getCombo(ppl, rs);
  }

  delete dialog;
}
