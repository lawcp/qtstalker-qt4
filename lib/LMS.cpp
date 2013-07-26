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

#include "LMS.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
#include <QVector>
//Added by qt3to4:
#include <Q3PtrList>
#include <math.h>

LMS::LMS()
{
  pluginName = "LMS";
  helpFile = "lms.html";

  colorKLabel = "colorK";
  color2DayLabel = "color2Day";
  color5DayLabel = "color5Day";
  labelLabel = "label";
  lineTypeKLabel = "lineTypeK";
  lineType2DayLabel = "lineType2Day";
  lineType5DayLabel = "lineType5Day";
  pluginLabel = "plugin";
  fkPeriodLabel = "fkPeriod";
  skPeriodLabel = "skPeriod";
  cmbIndexLabel = "cmbIndex";
  show2DayLabel = "show2Day";
  show5DayLabel = "show5Day";
  plotTestLabel = "plotTest";

  // format1: FK_PERIOD, SK_PERIOD, CMB_INDEX, SHOW_2DAY, SHOW_5DAY

  formatList.append(FormatInteger);
  formatList.append(FormatInteger);
  formatList.append(FormatString);
  formatList.append(FormatBool);
  formatList.append(FormatBool);

  setDefaults();
}

LMS::~LMS()
{
}

void LMS::setDefaults()
{
  colorK.setNamedColor("red");
  color2Day.setNamedColor("green");
  color5Day.setNamedColor("blue");
  lineTypeK = PlotLine::Line;
  lineType2Day = PlotLine::Line;
  lineType5Day = PlotLine::Line;
  label = pluginName;

  show2Day = TRUE;
  show5Day = FALSE;
  cycleFlag = TRUE;
  fkPeriod = 5;
  skPeriod = 5;
  cmbIndex = 1;

  //test pattern-- -- -----------------------------------------------------------------------
  testFlag = FALSE;
}

Indicator * LMS::calculate()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getLMS(pll);

  int loop;
  for (loop = 0; loop < (int) pll.count(); loop++)
    output->addLine(pll.at(loop));

  return output;
}

void LMS::getLMS (Q3PtrList<PlotLine> &pll)
{
  int i = 0;
  double sigPower = 0;
  double mu = 0;
  double xBar = 0;

  PlotLine *price = new PlotLine;

  //price = (h + l) / 2
  if (!testFlag)
  {
    for (i = 0; i < (int)data->count(); i++)
      price->append((data->getHigh(i) + data->getLow(i)) / 2);
  }
  //== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
    //Test pattern for cycle freaks:-)
  else
  {
    double Pi = 3.1415926;
    double twoPi = 2 * Pi;

    for (i = 0; i < 125; i++)
      price->append(25 * sin(twoPi * i / 10));

    PlotLine *testLine = new PlotLine;

    for (i = 0; i < 125; i++)
      testLine->append(1 + (0.2 * sin(twoPi * i / 10)));

    QString labelTest = "Price";
    QColor color;
    color.setNamedColor("yellow");

    testLine->setColor(color);
    testLine->setLabel(labelTest);
    pll.append(testLine);
  }

  //== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

  PlotLine *price_offset_4 = new PlotLine;
  //temps
  PlotLine *temp1 = new PlotLine;

  PlotLine *value1 = new PlotLine;
  //dual mode
  PlotLine *value2 = new PlotLine;
  PlotLine *value3 = new PlotLine;

  QVector< double > g(30);
  QVector< double > sigPredict(30);

  g.fill(0.0);
  sigPredict.fill(0.0);

  //Counter trend mode-- -- ------------------------------------------------------
  if (!cycleFlag)
  {
    int offset = 4;

    //price[4]
    for (i = 0; i < price->getSize(); i++)
    {
      if (i > offset - 1)
	price_offset_4->append(price->getData(i - offset));
    }

    //0.5(price - price[4]
    int priceIndex = price->getSize() - 1;
    for (i = price_offset_4->getSize() - 1; i >= 0; i--)
    {
      temp1->prepend(0.5 * (price->getData(priceIndex) - price_offset_4->getData(i)));
      priceIndex--;
    }

    //0.25(price + 0.5(price - price[4])
    priceIndex = price->getSize() - 1;
    for (i = temp1->getSize() - 1; i >= 0; i--)
    //work back for simplicity
    {
      temp1->setData(i, 0.25 * (temp1->getData(i) + price->getData(priceIndex)));
      priceIndex--;
    }

    //Value1 = .25 * (Price + .5 * (Price - Price[4])) + .75 * Value1[1];
    for (i = 0; i < temp1->getSize(); i++)
    {
      if (i == 0)
	value1->append(temp1->getData(i));
      else
	value1->append(temp1->getData(i) + (value1->getData(i - 1) * 0.75));
    }
  } //end counter trend mode

  // Cycle mode-- -- -----------------------------------------------------------------------
  else
  {
    //slowK(price, length, length);
    temp1 = slowK(price, fkPeriod, skPeriod);

    //0.2(2 * (SlowK(price, length, length) / 100 - .5))
    for (i = 0; i < temp1->getSize(); i++)
      temp1->setData(i, 0.2 * (2 * (temp1->getData(i)) / (100 - 0.5)));

    //Value1 = .2 * (2 * (SlowK(Length) / 100 - .5)) + .8 * Value1[1];
    for (i = 0; i < temp1->getSize(); i++)
    {
      if (i == 0)
	value1->append(temp1->getData(i));
      else
	value1->append(temp1->getData(i) + value1->getData(i - 1) * 0.8);
    }
  } //end cycle mode
    // end cycleFlag-- -- --------------------------------------------------------------------

  //if currentBar > length
  int j = 0;
  for (i = skPeriod; i < value1->getSize(); i++)
  {
    //Compute average power for normalization
    sigPower = 0;
    for (j = 0; j < skPeriod; j++)
      sigPower = sigPower + value1->getData(i - j) * value1->getData(i - j);

    sigPower /= skPeriod;

    //Convergence Factor
    if (sigPower > 0)
    mu = 0.25 / (sigPower * skPeriod);

    //Compute signal estimate
    xBar = 0;
    for (j = 1; j <= skPeriod; j++)
      xBar = xBar + value1->getData(i - j) * g[j];

    //Compute gain coefficients
    for (j = 1; j <= skPeriod; j++)
      g[j] = g[j] + (mu * (value1->getData(i) - xBar) * value1->getData(i - j));

    //Compute signal prediction waveform
    for (j = 0; j <= skPeriod; j++)
      sigPredict[j] = value1->getData(i - (skPeriod - j));

    //Extend signal prediction into the future
    int k = 0;
    for (j = skPeriod + 1; j <= skPeriod + 5; j++)
    {
      sigPredict[j] = 0;

      for (k = 1; k <= skPeriod; k++)
	sigPredict[j] = sigPredict[j] + sigPredict[j - k] * g[k];
    }

    value2->append(sigPredict[skPeriod + 2]);
    value3->append(sigPredict[skPeriod + 5]);
  }

  delete price;
  delete price_offset_4;
  delete temp1;

  //remove the first 1.5 * length values-- initial ramp up messes with scaling
  // ***n.b need a delete(index) function for PlotLine ***
  PlotLine *temp1a = new PlotLine;
  PlotLine *temp2 = new PlotLine;
  PlotLine *temp3 = new PlotLine;

  int index1 = value1->getSize() -1;
  int index2 = value2->getSize() - 1;
  int index3 = value3->getSize() - 1;

  for (i = value2->getSize() - 1; i >= (int)skPeriod * 1.5; i--)
  {
    temp1a->prepend(value1->getData(index1));
    temp2->prepend(value2->getData(index2));
    temp3->prepend(value3->getData(index3));

    index1--;
    index2--;
    index3--;
  }

  delete value1;
  delete value2;
  delete value3;

  value1 = temp1a;
  value2 = temp2;
  value3 = temp3;

  //output results:

  QString labelx = "SlowK";

  value1->setColor(colorK);
  value1->setLabel(labelx);
  value1->setType(lineTypeK);
  pll.append(value1);

  if (show2Day)
  {
    labelx = "2Day";
    value2->setColor(color2Day);
    value2->setLabel(labelx);
    value2->setType(lineType2Day);
    pll.append(value2);
  }
  else
    delete value2;

  if (show5Day)
  {
    labelx = "5Day";
    value3->setColor(color5Day);
    value3->setLabel(labelx);
    value3->setType(lineType5Day);
    pll.append(value3);
  }
  else
    delete value3;
}

PlotLine *LMS::slowK(PlotLine * inLine, int kPeriod, int slowKperiod)
{
  PlotLine *k = new PlotLine();
  int loop;
  for (loop = kPeriod; loop < (int)inLine->getSize(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < kPeriod; loop2++)
    {
      double t = inLine->getData(loop - loop2);
      if (t > h)
	h = t;
      if (t < l)
	l = t;
    }

    double close = inLine->getData(loop);
    double t = ((close - l) / (h - l)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    k->append(t);
  }

  PlotLine *k2 = getMA(k, 0, slowKperiod);
  delete k;
  k = k2;

  return k;
}

int LMS::indicatorPrefDialog(QWidget * w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("SlowK Color");
  QString cl2 = QObject::tr("2 Day prediction Color");
  QString cl3 = QObject::tr("5 Day Prediction Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type K");
  QString lt2 = QObject::tr("Line Type 2 Day");
  QString lt3 = QObject::tr("Line Type 5 Day");
  QString ck1 = QObject::tr("Show 2 Day Prediction");
  QString ck2 = QObject::tr("Show 5 Day Prediction");
  QString fk = QObject::tr("Fast K Period");
  QString sk = QObject::tr("Slow K Period");
  QString cmb = QObject::tr("Select Mode");

  //test pattern-- -- -----------------------------------------------------------------------
  QString ck3 = QObject::tr("Plot Test Pattern");
  //----------------------------------------------------------------------------------------

  PrefDialog * dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("LMS Indicator"));
  dialog->createPage(pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, colorK);
  dialog->addComboItem(ltl, pl, lineTypes, lineTypeK);
  dialog->addColorItem(cl2, pl, color2Day);
  dialog->addComboItem(lt2, pl, lineTypes, lineType2Day);
  dialog->addColorItem(cl3, pl, color5Day);
  dialog->addComboItem(lt3, pl, lineTypes, lineType5Day);
  dialog->addTextItem(ll, pl, label);

  QStringList l;
  l << "Counter Trend" << "Cycle";
  dialog->addComboItem(cmb, pl, l, cycleFlag);
  dialog->addIntItem(fk, pl, fkPeriod, 2, 99999999);
  dialog->addIntItem(sk, pl, skPeriod, 2, 99999999);
  dialog->addCheckItem(ck1, pl, show2Day);
  dialog->addCheckItem(ck2, pl, show5Day);

  //test pattern-- -- -----------------------------------------------------------------------
  dialog->addCheckItem(ck3, pl, testFlag);
  //----------------------------------------------------------------------------------------

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, colorK);
    dialog->getColor(cl2, color2Day);
    dialog->getColor(cl3, color5Day);
    lineTypeK = (PlotLine::LineType) dialog->getComboIndex(ltl);
    lineType2Day = (PlotLine::LineType) dialog->getComboIndex(lt2);
    lineType5Day = (PlotLine::LineType) dialog->getComboIndex(lt3);
    dialog->getText(ll, label);
    show2Day = dialog->getCheck(ck1);
    show5Day = dialog->getCheck(ck2);
    cmbIndex = dialog->getComboIndex(cmb);

    if (cmbIndex == 1)
      //0 = counter tredn, 1 = cycle
      cycleFlag = TRUE;
    else
      cycleFlag = FALSE;

    //test pattern-- -- -----------------------------------------------------------------------
    testFlag = dialog->getCheck(ck3);
    //----------------------------------------------------------------------------------------

    fkPeriod = dialog->getInt(fk);
    skPeriod = dialog->getInt(sk);

    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

PlotLine *LMS::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: FK_PERIOD, SK_PERIOD, CMB_INDEX, SHOW_2DAY, SHOW_5DAY

  if (checkFormat(p, d, 5, 5))
    return 0;

  fkPeriod = formatStringList[0].toInt();
  skPeriod = formatStringList[1].toInt();

  if (! formatStringList[2].compare("Cycle"))
  {
    cmbIndex = 1;
    cycleFlag = TRUE;
  }
  else
  {
    if (! formatStringList[2].compare("Counter Trend"))
    {
      cmbIndex = 0;
      cycleFlag = FALSE;
    }
    else
    {
      qDebug("LMS::calculateCustom: invalid CMB_INDEX parm");
      return 0;
    }
  }

  if (! formatStringList[3].compare("TRUE"))
    show2Day = TRUE;
  else
    show2Day = FALSE;

  if (! formatStringList[4].compare("TRUE"))
    show5Day = TRUE;
  else
    show5Day = FALSE;

  Q3PtrList<PlotLine> pll;
  pll.setAutoDelete(FALSE);
  getLMS(pll);

  int loop;
  for (loop = pll.count() - 1; loop > 0; loop--)
    pll.remove(loop);

  return pll.at(0);
}

void LMS::getIndicatorSettings(Setting & dict)
{
  QString ts = colorK.name();
  dict.setData(colorKLabel, ts);
  ts = color2Day.name();
  dict.setData(color2DayLabel, ts);
  ts = color5Day.name();
  dict.setData(color5DayLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(lineTypeK);
  dict.setData(lineTypeKLabel, ts);
  ts = QString::number(lineType2Day);
  dict.setData(lineType2DayLabel, ts);
  ts = QString::number(lineType5Day);
  dict.setData(lineType5DayLabel, ts);
  dict.setData(pluginLabel, pluginName);
  ts = QString::number(fkPeriod);
  dict.setData(fkPeriodLabel, ts);
  ts = QString::number(skPeriod);
  dict.setData(skPeriodLabel, ts);
  ts = QString::number(cmbIndex);
  dict.setData(cmbIndexLabel, ts);
  ts = QString::number(show2Day);
  dict.setData(show2DayLabel, ts);
  ts = QString::number(show5Day);
  dict.setData(show5DayLabel, ts);
  ts = QString::number(testFlag);
  dict.setData(plotTestLabel, ts);
}

void LMS::setIndicatorSettings(Setting & dict)
{
  setDefaults();

  if (!dict.count())
    return;

  QString s;
  dict.getData(colorKLabel, s);
  if (s.length())
    colorK.setNamedColor(s);

  dict.getData(color2DayLabel, s);
  if (s.length())
    color2Day.setNamedColor(s);

  dict.getData(color5DayLabel, s);
  if (s.length())
    color5Day.setNamedColor(s);

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(lineTypeKLabel, s);
  if (s.length())
    lineTypeK = (PlotLine::LineType) s.toInt();

  dict.getData(lineType2DayLabel, s);
  if (s.length())
    lineType2Day = (PlotLine::LineType) s.toInt();

  dict.getData(lineType5DayLabel, s);
  if (s.length())
    lineType5Day = (PlotLine::LineType) s.toInt();

  dict.getData(fkPeriodLabel, s);
  if (s.length())
    fkPeriod = s.toInt();

  dict.getData(skPeriodLabel, s);
  if (s.length())
    skPeriod = s.toInt();

  dict.getData(cmbIndexLabel, s);
  if (s.length())
    cmbIndex = s.toInt();

  if (cmbIndex == 1)
    cycleFlag = TRUE;
  else
    cycleFlag = FALSE;
  dict.getData(show2DayLabel, s);
  if (s.length())
    show2Day = s.toInt();

  dict.getData(show5DayLabel, s);
  if (s.length())
    show5Day = s.toInt();

  dict.getData(plotTestLabel, s);
  if (s.length())
    testFlag = s.toInt();
}

void LMS::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vl = QObject::tr("Variable Name");
  QString ck1 = QObject::tr("Show 2 Day Prediction");
  QString ck2 = QObject::tr("Show 5 Day Prediction");
  QString fk = QObject::tr("Fast K Period");
  QString sk = QObject::tr("Slow K Period");
  QString cmb = QObject::tr("Select Mode");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("LMS Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vl, pl, s);
  dialog->addIntItem(fk, pl, fkPeriod, 1, 999999);
  dialog->addIntItem(sk, pl, skPeriod, 1, 999999);
  QStringList l;
  l.append("Cycle");
  l.append("Counter Trend");
  dialog->addComboItem(cmb, pl, l, 0);
  dialog->addCheckItem(ck1, pl, show2Day);
  dialog->addCheckItem(ck2, pl, show5Day);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vl, rv);

    int t = dialog->getInt(fk);
    rs.append(QString::number(t) + ",");

    t = dialog->getInt(sk);
    rs.append(QString::number(t) + ",");

    dialog->getCombo(cmb, s);
    rs.append(s + ",");

    t = dialog->getCheck(ck1);
    if (t)
      rs.append("TRUE,");
    else
      rs.append("FALSE,");

    t = dialog->getCheck(ck2);
    if (t)
      rs.append("TRUE");
    else
      rs.append("FALSE");
  }

  delete dialog;
}
