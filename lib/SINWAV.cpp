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

#include "SINWAV.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <math.h>
#include <QVector>

#define PI 3.14159265

SINWAV::SINWAV ()
{
  pluginName = "SINWAV";
  helpFile = "sinwav.html";

  colorSinLabel = "colorSin";
  colorLeadLabel = "colorLead";
  labelSinLabel = "labelSin";
  labelLeadLabel = "labelLead";
  lineTypeSinLabel = "lineTypeSin";
  lineTypeLeadLabel = "lineTypeLead";
  pluginLabel = "plugin";

  setDefaults();
}

SINWAV::~SINWAV ()
{
}

void SINWAV::setDefaults ()
{
  colorSin.setNamedColor("red");
  colorLead.setNamedColor("blue");
  lineTypeSin = PlotLine::Line;
  lineTypeLead = PlotLine::Line;
  labelSin = pluginName;
  labelLead = "Lead";
}

Indicator * SINWAV::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getSINWAV(pll);

  int loop;
  for (loop = 0; loop < (int) pll.count(); loop++)
    output->addLine(pll.at(loop));

  return output;
}

void SINWAV::getSINWAV (Q3PtrList<PlotLine> &pll)
{
  // Ehler's sine wave

  PlotLine *Price = new PlotLine;

  int i = 0;

  // price = (h + l) / 2
  for (i = 0; i < (int) data->count(); i++)
    Price->append((data->getHigh(i) + data->getLow(i)) / 2);

  //! VERSION 2

  QVector<double> *smooth = new QVector<double>(Price->getSize());
  smooth->fill(0.0);
  QVector<double> *detrender = new QVector<double>(Price->getSize());
  detrender->fill(0.0);
  QVector<double> *period = new QVector<double>(Price->getSize());
  period->fill(0.0);
  QVector<double> *Q1 = new QVector<double>(Price->getSize());
  Q1->fill(0.0);
  QVector<double> *I1 = new QVector<double>(Price->getSize());
  I1->fill(0.0);
  QVector<double> *jI = new QVector<double>(Price->getSize());
  jI->fill(0.0);
  QVector<double> *jQ = new QVector<double>(Price->getSize());
  jQ->fill(0.0);
  QVector<double> *I2 = new QVector<double>(Price->getSize());
  I2->fill(0.0);
  QVector<double> *Q2 = new QVector<double>(Price->getSize());
  Q2->fill(0.0);
  QVector<double> *Re = new QVector<double>(Price->getSize());
  Re->fill(0.0);
  QVector<double> *Im = new QVector<double>(Price->getSize());
  Im->fill(0.0);
  QVector<double> *SmoothPrice = new QVector<double>(Price->getSize());
  SmoothPrice->fill(0.0);
  QVector<double> *DCPhase = new QVector<double>(Price->getSize());
  DCPhase->fill(0.0);

  PlotLine *out1 = new PlotLine;
  PlotLine *out2 = new PlotLine;

  double SmoothPeriod = 0;
  double DCPeriod = 0;

  for (i = 6; i< Price->getSize(); i++)
  {
    // Weighted price
    (*smooth)[i] = ( 4 * Price->getData(i) + 3 * Price->getData(i-1) +
		      2 * Price->getData(i-2) +  Price->getData(i-3)) /10.0;

    (*detrender)[i] = (0.0962 * smooth->at(i) + 0.5769 * smooth->at(i-2) -
			0.5769 * smooth->at(i-4) - 0.0962 * smooth->at(i-6)) *
		        (0.075 * period->at(i-1) + 0.54);

    // Compute InPhase and Quadrature components
    (*Q1)[i] = (0.0962 * detrender->at(i) + 0.5769 * detrender->at(i-2) -
		 0.5769 * detrender->at(i-4) - 0.0962 * detrender->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);
    (*I1)[i] = detrender->at(i-3);

    //Advance the phase of I1 and Q1 by 90 degrees
    (*jI)[i] = (0.0962 * I1->at(i) + 0.5769 * I1->at(i-2) -
		 0.5769 * I1->at(i-4) - 0.0962 * I1->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);

    (*Q1)[i] = (0.0962 * Q1->at(i) + 0.5769 * Q1->at(i-2) -
		 0.5769 * Q1->at(i-4) - 0.0962 * Q1->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);

    // Phasor addition for 3-bar averaging
    (*I2)[i] = I1->at(i) - jQ->at(i);
    (*Q2)[i] = Q1->at(i) + jI->at(i);

    // Smooth the I and Q components before applying the discriminator
    (*I2)[i] = 0.2 * I2->at(i) + 0.8 * I2->at(i-1);
    (*Q2)[i] = 0.2 * Q2->at(i) + 0.8 * Q2->at(i-1);

    // Homodyne Discriminator
    (*Re)[i] = I2->at(i) * I2->at(i-1) + Q2->at(i) * Q2->at(i-1);
    (*Im)[i] = I2->at(i) * Q2->at(i-1) - Q2->at(i) * I2->at(i-1);
    (*Re)[i] = 0.2 * Re->at(i) + 0.8 * Re->at(i-1);
    (*Im)[i] = 0.2 * Im->at(i) + 0.8 * Im->at(i-1);
    if (Im->at(i) != 0 && Re->at(i) != 0 )
      (*period)[i] = 360/(atan(Im->at(i) / Re->at(i)) * (180/PI));
    if (period->at(i) > 1.5 * period->at(i-1))
      (*period)[i] = 1.5 * period->at(i-1);
    if (period->at(i) < 0.67 * period->at(i-1))
      (*period)[i] = 0.67 * period->at(i-1);
    if (period->at(i) < 6)
      (*period)[i] = 6;
    if (period->at(i) > 50)
      (*period)[i] = 50;

    (*period)[i] = 0.2 * period->at(i) + 0.8 * period->at(i-1);

    SmoothPeriod = 0.33 * period->at(i) + 0.67 * SmoothPeriod;

    // Compute Dominant CyclePhase
    (*SmoothPrice)[i] = (4 * Price->getData(i) + 3 * Price->getData(i-1) +
			  2 * Price->getData(i-2) +  Price->getData(i-3)) /10.0;

    DCPeriod = (int)(SmoothPeriod + 0.5);

    double RealPart = 0;
    double ImagPart = 0;
    int j = 0;

    if (i >= (int)DCPeriod)
    {
      for (j = 0; j < (int)DCPeriod; j++)
      {
        RealPart = RealPart + sin((360 * (double)j / DCPeriod) * (PI/180)) * (SmoothPrice->at(i - j));
	ImagPart = ImagPart + cos((360 * (double)j / DCPeriod) * (PI/180)) * (SmoothPrice->at(i - j));
      }
    }

    if (fabs(ImagPart) > 0.001)
      (*DCPhase)[i] = atan(RealPart / ImagPart) * (180/PI);

    if( fabs(ImagPart) <= 0.001 )
      (*DCPhase)[i] = 90.0 * sin(RealPart * (PI/180));

    (*DCPhase)[i] = DCPhase->at(i) + 90.0;

    // Compensate for one bar lag of the Weighted Moving Average
    (*DCPhase)[i] = DCPhase->at(i) + 360.0 / SmoothPeriod;

    if (ImagPart < 0)
      (*DCPhase)[i] = DCPhase->at(i) + 180.0;

    if (DCPhase->at(i) > 315.0)
      (*DCPhase)[i] = DCPhase->at(i) - 360.0;

    // we're done....
    out1->append(sin(DCPhase->at(i) * (PI/180)));
    out2->append(sin((DCPhase->at(i) + 45.0) * (PI/180)));
  }

  delete smooth;
  delete detrender;
  delete period;
  delete Q1;
  delete I1;
  delete jI;
  delete jQ;
  delete I2;
  delete Q2;
  delete Re;
  delete Im;
  delete SmoothPrice;
  delete DCPhase;
  delete Price;

  out1->setColor(colorSin);
  out1->setLabel(labelSin);
  out1->setType(lineTypeSin);
  pll.append(out1);

  out2->setColor(colorLead);
  out2->setLabel(labelLead);
  out2->setType(lineTypeLead);
  pll.append(out2);
}

int SINWAV::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Sine Color");
  QString c2 = QObject::tr("Lead Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Sine Line Type");
  QString lt2 = QObject::tr("Lead Line Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("SINWAV Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, colorSin);
  dialog->addComboItem(ltl, pl, lineTypes, lineTypeSin);
  dialog->addColorItem(c2, pl, colorLead);
  dialog->addComboItem(lt2, pl, lineTypes, lineTypeLead);
  dialog->addTextItem(ll, pl, labelSin);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, colorSin);
    dialog->getColor(c2, colorLead);
    lineTypeSin = (PlotLine::LineType) dialog->getComboIndex(ltl);
    lineTypeLead = (PlotLine::LineType) dialog->getComboIndex(lt2);
    dialog->getText(ll, labelSin);

    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

PlotLine * SINWAV::calculateCustom (QString &, Q3PtrList<PlotLine> &)
{
  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getSINWAV(pll);
  pll.remove(1);
  return pll.at(0);
}

void SINWAV::getIndicatorSettings (Setting &dict)
{
  QString ts = colorSin.name();
  dict.setData(colorSinLabel, ts);
  ts = colorLead.name();
  dict.setData(colorLeadLabel, ts);
  dict.setData(labelSinLabel, labelSin);
  dict.setData(labelLeadLabel, labelLead);
  ts = QString::number(lineTypeSin);
  dict.setData(lineTypeSinLabel, ts);
  ts = QString::number(lineTypeLead);
  dict.setData(lineTypeLeadLabel, ts);
  dict.setData(pluginLabel, pluginName);
}

void SINWAV::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getData(colorSinLabel, s);
  if (s.length())
    colorSin.setNamedColor(s);

  dict.getData(colorLeadLabel, s);
  if (s.length())
    colorLead.setNamedColor(s);

  dict.getData(labelSinLabel, s);
  if (s.length())
    labelSin = s;

  dict.getData(labelLeadLabel, s);
  if (s.length())
    labelLead = s;

  dict.getData(lineTypeSinLabel, s);
  if (s.length())
    lineTypeSin = (PlotLine::LineType) s.toInt();

  dict.getData(lineTypeLeadLabel, s);
  if (s.length())
    lineTypeLead = (PlotLine::LineType) s.toInt();
}

void SINWAV::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("SINWAV Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    dialog->getText(vnl, rv);

  delete dialog;
}
