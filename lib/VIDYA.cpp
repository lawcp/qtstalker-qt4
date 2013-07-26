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

#include "VIDYA.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <math.h>
#include <QVector>

#define PI 3.14159265

VIDYA::VIDYA ()
{
  pluginName = "VIDYA";
  helpFile = "vidya.html";

  colorLabel = "color";
  labelLabel = "label";
  lineTypeLabel = "lineType";
  pluginLabel = "plugin";
  periodLabel = "period";
  volPeriodLabel = "volPeriod";
  inputLabel = "input";

  formatList.append(FormatInputArray);
  formatList.append(FormatInteger);
  formatList.append(FormatInteger);

  setDefaults();
}

VIDYA::~VIDYA ()
{
}

void VIDYA::setDefaults ()
{
  color.setNamedColor("cyan");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
  volPeriod = 10;
  input = BarData::Close;
}

Indicator * VIDYA::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("VIDYA::calculate: no input");
    return output;
  }

  if ( in->getSize() < period )
  {
    qDebug("VIDYA::calculate: insufficient data");
    return output;
  }

  PlotLine *out = new PlotLine;

  calcVidya (out, in, volPeriod, period);

  out->setColor(color);
  out->setType(lineType);
  out->setLabel(pluginName);
  output->addLine(out);
  return output;
}

void VIDYA::calcVidya (PlotLine *outSignal, PlotLine *inSignal, int iCmoPeriod, int iVidyaPeriod)
{
  PlotLine *cmo = new PlotLine;

  calcCMO(cmo, inSignal, iCmoPeriod);

  int i = 0;
  int loop = (int)inSignal->getSize();

  QVector<double> *inSeries = new QVector<double>(loop);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(loop);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(loop);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(loop);
  vidya->fill(0.0);

  double c = 2 / (double)iVidyaPeriod +1;

  for ( i = 0; i < loop; i++)
    (*inSeries)[i] = inSignal->getData(i);

  int index = inSeries->size() -1;
  for (i = cmo->getSize() -1; i >= 0; i--)
  {
    (*absCmo)[index] = fabs(cmo->getData(i) / 100);
    index--;
  }

  for (i = iCmoPeriod + iVidyaPeriod; i < (int)inSeries->size(); i++)		// period safty
  {
    (*vidya)[i] = ( inSeries->at(i) * c * absCmo->at(i) ) + ( (1 - absCmo->at(i) * c) * vidya->at(i-1) );
    //!  (Price*Const*AbsCMO) + ((1-AbsCMO*Const)*VIDYA[1]),Price);
    outSignal->append(vidya->at(i));
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;
}

void VIDYA::calcCMO ( PlotLine *outSignal, PlotLine *inSignal, int iPeriod)
{
  //!  Chande Momentum Oscillator
  //!  Raw VIDYA

  int loop = (int)inSignal->getSize();

  QVector<double> *inSeries = new QVector<double>(loop);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(loop);
  offset->fill(0.0);
  QVector<double> *mom = new QVector<double>(loop);
  mom->fill(0.0);
  QVector<double> *posSeries = new QVector<double>(loop);
  posSeries->fill(0.0);
  QVector<double> *negSeries = new QVector<double>(loop);
  negSeries->fill(0.0);
  QVector<double> *sumPos = new QVector<double>(loop);
  sumPos->fill(0.0);
  QVector<double> *sumNeg = new QVector<double>(loop);
  sumNeg->fill(0.0);
  QVector<double> *cmoUp = new QVector<double>(loop);
  cmoUp->fill(0.0);
  QVector<double> *cmoDown = new QVector<double>(loop);
  cmoDown->fill(0.0);
  QVector<double> *rawCmo = new QVector<double>(loop);
  rawCmo->fill(0.0);

  int i = 0;

  for (i = 0; i < loop; i++)
    (*inSeries)[i] = inSignal->getData(i);

  for (i = iPeriod - 1; i < loop; i++)
  {
    (*offset)[i] = inSeries->at(i-1);

    (*mom)[i] = inSeries->at(i) - offset->at(i);

    if (mom->at(i) > 0)
      (*posSeries)[i] = mom->at(i);
    else
      (*posSeries)[i] = 0;

    if (mom->at(i) < 0)
      (*negSeries)[i] = fabs(mom->at(i));
    else
      (*negSeries)[i] = 0;

    int j = 0;
    double sumUp = 0;
    double sumDown = 0;

    for (j = 0; j < iPeriod; j++)
    {
      sumUp += posSeries->at(i-j);
      sumDown += negSeries->at(i-j);
    }

    (*sumPos)[i] = sumUp;
    (*sumNeg)[i] = sumDown;

    (*cmoUp)[i] = 100 * ((sumPos->at(i) - sumNeg->at(i)));

    (*cmoDown)[i] = sumPos->at(i) + sumNeg->at(i);

    (*rawCmo)[i] = cmoUp->at(i) / cmoDown->at(i);

    if (i > iPeriod -1)
      outSignal->append(rawCmo->at(i));
  }

  delete inSeries;
  delete offset;
  delete mom;
  delete posSeries;
  delete negSeries;
  delete sumPos;
  delete sumNeg;
  delete cmoUp;
  delete cmoDown;
  delete rawCmo;
}

void VIDYA::calcAdaptCMO ( PlotLine *outSignal, PlotLine *inSignal, int iStdPeriod,
                           int iMinLook, int iMaxLook)
{
  //! Chande Momentum Oscillator
  //! Adaptaave VIDYA
  //! Not used here, but it has possibilities....

  PlotLine *currentLookback = new PlotLine;

  getStdDev(currentLookback, inSignal, iStdPeriod);

  getNorm( currentLookback, iMinLook, iMaxLook );

  int i = 0;

  for (i = 0; i < currentLookback->getSize(); i++)
    currentLookback->setData( i, (int)currentLookback->getData(i) );

  int loop = (int)inSignal->getSize();

  QVector<double> *inSeries = new QVector<double>(loop);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(loop);
  offset->fill(0.0);
  QVector<double> *mom = new QVector<double>(loop);
  mom->fill(0.0);
  QVector<double> *posSeries = new QVector<double>(loop);
  posSeries->fill(0.0);
  QVector<double> *negSeries = new QVector<double>(loop);
  negSeries->fill(0.0);
  QVector<double> *sumPos = new QVector<double>(loop);
  sumPos->fill(0.0);
  QVector<double> *sumNeg = new QVector<double>(loop);
  sumNeg->fill(0.0);
  QVector<double> *cmoUp = new QVector<double>(loop);
  cmoUp->fill(0.0);
  QVector<double> *cmoDown = new QVector<double>(loop);
  cmoDown->fill(0.0);
  QVector<double> *currentLook = new QVector<double>(loop);
  currentLook->fill(0.0);
  QVector<double> *adaptCmo = new QVector<double>(loop);
  adaptCmo->fill(0.0);

  // line up data
  int index = currentLook->size() -1;
  for (i = currentLookback->getSize() -1; i >=0; i--)
  {
    (*currentLook)[index] = currentLookback->getData(i);
    index--;
  }

  for (i = 0; i < loop; i++)
    (*inSeries)[i] = inSignal->getData(i);

  for (i = iStdPeriod - 1; i < loop; i++)
  {
    (*offset)[i] = inSeries->at(i-1);

    (*mom)[i] = inSeries->at(i) - offset->at(i);

    if (mom->at(i) > 0)
      (*posSeries)[i] = mom->at(i);
    else
      (*posSeries)[i] = 0;

    if (mom->at(i) < 0)
      (*negSeries)[i] = fabs(mom->at(i));
    else
      (*negSeries)[i] = 0;

    int j = 0;
    double sumUp = 0;
    double sumDown = 0;

    for (j = 0; j < (int)currentLook->at(i); j++)
    {
      sumUp += posSeries->at(i-j);
      sumDown += negSeries->at(i-j);
    }

    (*sumPos)[i] = sumUp;
    (*sumNeg)[i] = sumDown;

    (*cmoUp)[i] = 100 * ((sumPos->at(i) - sumNeg->at(i)));

    (*cmoDown)[i] = sumPos->at(i) + sumNeg->at(i);

    (*adaptCmo)[i] = cmoUp->at(i) / cmoDown->at(i);

    if (i > iStdPeriod -1)
      outSignal->append(adaptCmo->at(i));
  }

  delete inSeries;
  delete offset;
  delete mom;
  delete posSeries;
  delete negSeries;
  delete sumPos;
  delete sumNeg;
  delete cmoUp;
  delete cmoDown;
  delete adaptCmo;
  delete currentLook;
  delete currentLookback;
}

void VIDYA::getStdDev( PlotLine *outLine, PlotLine *inLine,  int iPeriod )
{
  int loop;

  for (loop = iPeriod -1; loop < (int) inLine->getSize(); loop++)
  {
    double mean = 0;
    int loop2;
    for (loop2 = 0; loop2 < iPeriod; loop2++)
      mean += inLine->getData(loop - loop2);

    mean /= (double)iPeriod;

    double ds = 0;
    for (loop2 = 0; loop2 < iPeriod; loop2++)
    {
      double t = inLine->getData(loop - loop2) - mean;
      ds += (t * t);
    }

    ds = sqrt(ds / (double)period);
    outLine->append(ds);
  }
}

void VIDYA::getNorm( PlotLine *inSig, double iMin, double iMax)
{
  //	I = Imin + (Imax-Imin)*(V-Vmin)/(Vmax-Vmin)
  int i = 0;
  double max = -999999;
  double min = 999999;
  double norm = 0;

  for (i = 0; i < inSig->getSize(); i++)
  {
    if(inSig->getData(i) > max)
      max = inSig->getData(i);

    if(inSig->getData(i) < min)
      min = inSig->getData(i);
  }
  // double range = fabs(max) + fabs(min);

  for (i = 0; i < inSig->getSize(); i++)
  {
    norm = iMin + (iMax - iMin) * ( (inSig->getData(i) - min) / (max - min) );
    inSig->setData(i, norm);
  }
}

int VIDYA::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Vidya period");
  QString per2 = QObject::tr("Volatility Period");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VIDYA Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addIntItem(perl, pl, period, 2, 50);
  dialog->addIntItem(per2, pl, volPeriod, 2, 50);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addComboItem(il, pl, inputTypeList, input);

  pl = QObject::tr("Zones");
  dialog->createPage (pl);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    dialog->getText(ll, label);
    period = dialog->getInt(perl);
    volPeriod = dialog->getInt(per2);
    input = (BarData::InputType) dialog->getComboIndex(il);

    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;

  return rc;
}

PlotLine * VIDYA::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, PERIOD, VOL_PERIOD

  if (checkFormat(p, d, 3, 3))
    return 0;

  period = formatStringList[1].toInt();
  volPeriod = formatStringList[2].toInt();

  PlotLine *out = new PlotLine;
  calcVidya (out, d.at(0), volPeriod, period);
  return out;
}

void VIDYA::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(pluginLabel, pluginName);
  ts = QString::number(period);
  dict.setData(periodLabel, ts);
  ts = QString::number(volPeriod);
  dict.setData(volPeriodLabel, ts);
  ts = QString::number(input);
  dict.setData(inputLabel, ts);
}

void VIDYA::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getData(colorLabel, s);
  if (s.length())
    color.setNamedColor(s);

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(periodLabel, s);
  if (s.length())
    period = s.toInt();

  dict.getData(volPeriodLabel, s);
  if (s.length())
    volPeriod = s.toInt();

  dict.getData(inputLabel, s);
  if (s.length())
    input = (BarData::InputType) s.toInt();
}

void VIDYA::formatDialog (QStringList &vl, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString perl = QObject::tr("Vidya period");
  QString per2 = QObject::tr("Volatility Period");
  QString il = QObject::tr("Input");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("VIDYA Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  // format1: ARRAY_INPUT, PERIOD, VOL_PERIOD

  QString s;
  dialog->addTextItem(vnl, pl, s);
  dialog->addComboItem(il, pl, vl, input);
  dialog->addIntItem(perl, pl, period, 2, 50);
  dialog->addIntItem(per2, pl, volPeriod, 2, 50);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);

    dialog->getCombo(il, rs);

    int t = dialog->getInt(perl);
    rs.append("," + QString::number(t));

    t = dialog->getInt(per2);
    rs.append("," + QString::number(t));
  }

  delete dialog;
}
