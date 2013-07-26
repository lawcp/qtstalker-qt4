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

#include "UTIL.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qinputdialog.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <math.h>

UTIL::UTIL ()
{
  pluginName = "UTIL";

  methodList.append("ACCUM");
  methodList.append("ADD");
  methodList.append("COLOR");
  methodList.append("COMP");
  methodList.append("COUNTER");
  methodList.append("DIV");
  methodList.append("Higher");
  methodList.append("INRANGE");
  methodList.append("Lower");
  methodList.append("MUL");
  methodList.append("Normal");
  methodList.append("PER");
  methodList.append("REF");
  methodList.append("SIGN");
  methodList.append("SUB");
  methodList.sort();

  helpFile = "math.html";
}

UTIL::~UTIL ()
{
}

PlotLine * UTIL::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() > 0)
    ;
  else
  {
    qDebug("UTIL::Custom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("UTIL::Custom: invalid METHOD parm");
    return 0;
  }

  PlotLine *out = 0;

  while (1)
  {
    if (! l[0].compare("ACCUM"))
    {
      out = calculateAccum(p, d);
      break;
    }

    if (! l[0].compare("ADD"))
    {
      out = calculateADMS(p, d, 0);
      break;
    }

    if (! l[0].compare("COLOR"))
    {
      out = calculateCOLOR(p, d);
      break;
    }

    if (! l[0].compare("COMP"))
    {
      out = calculateCOMP(p, d);
      break;
    }

    if (! l[0].compare("COUNTER"))
    {
      out = calculateCOUNTER(p, d);
      break;
    }

    if (! l[0].compare("DIV"))
    {
      out = calculateADMS(p, d, 1);
      break;
    }

    if (! l[0].compare("Higher"))
    {
      out = calculateHL(p, d, 1);
      break;
    }

    if (! l[0].compare("INRANGE"))
    {
      out = calculateINRANGE(p, d);
      break;
    }

    if (! l[0].compare("Lower"))
    {
      out = calculateHL(p, d, 2);
      break;
    }

    if (! l[0].compare("MUL"))
    {
      out = calculateADMS(p, d, 2);
      break;
    }

    if (! l[0].compare("Normal"))
    {
      out = calculateNormal(p, d);
      break;
    }

    if (! l[0].compare("PER"))
    {
      out = calculatePER(p, d);
      break;
    }

    if (! l[0].compare("REF"))
    {
      out = calculateREF(p, d);
      break;
    }

    if (! l[0].compare("SIGN"))
    {
      out = calculateSIGN(p, d);
      break;
    }

    if (! l[0].compare("SUB"))
    {
      out = calculateADMS(p, d, 3);
      break;
    }

    break;
  }

  return out;
}

PlotLine * UTIL::calculateADMS (QString &p, Q3PtrList<PlotLine> &d, int type)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);
  formatList.append(FormatString); // dummy cause we dont know what this parm is beforehand

  if (checkFormat(p, d, 3, 3))
    return 0;

  PlotLine *input = d.at(0);
  PlotLine *input2 = 0;
  if (d.count() == 2)
    input2 = d.at(1);

  double inputNum = 0;
  if (! input2)
  {
    bool ok;
    double t = formatStringList[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
      return 0;
  }

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;

  PlotLine *line = new PlotLine;

  while (loop > -1)
  {
    double v = input->getData(loop);

    if (input2)
    {
      if (loop2 < 0)
        break;
      switch(type)
      {
        case 0:
          v = v + input2->getData(loop2);
          break;
        case 1:
          v = v / input2->getData(loop2);
          break;
        case 2:
          v = v * input2->getData(loop2);
          break;
        case 3:
          v = v - input2->getData(loop2);
          break;
        default:
          break;
      }
      loop2--;
    }
    else
    {
      switch(type)
      {
        case 0:
          v = v + inputNum;
          break;
        case 1:
          v = v / inputNum;
          break;
        case 2:
          v = v * inputNum;
          break;
        case 3:
          v = v - inputNum;
          break;
        default:
          break;
      }
    }

    line->prepend(v);
    loop--;
  }

  return line;
}

PlotLine * UTIL::calculateAccum (QString &p, Q3PtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);

  if (checkFormat(p, d, 2, 2))
    return 0;

  PlotLine *line = new PlotLine;
  PlotLine *input = d.at(0);

  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) input->getSize(); loop++)
  {
    accum = accum + input->getData(loop);
    line->append(accum);
  }

  return line;
}

PlotLine * UTIL::calculateCOLOR (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);
  formatList.append(FormatInputArray2);
  formatList.append(FormatInteger);
  formatList.append(FormatString);

  if (checkFormat(p, d, 5, 5))
    return 0;

  int value = formatStringList[3].toInt();

  QColor c(formatStringList[4]);
  if (! c.isValid())
  {
    qDebug("UTIL::COLOR: invalid COLOR parm");
    return 0;
  }

  PlotLine *inbool = d.at(0);
  int inboolLoop = inbool->getSize() - 1;
  PlotLine *incol = d.at(1);
  incol->setColorFlag(TRUE);
  int incolLoop = incol->getSize() - 1;
  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (inbool->getData(inboolLoop) == value)
      incol->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }

  PlotLine *line = new PlotLine;
  return line;
}

PlotLine * UTIL::calculateCOMP (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE, OPERATOR
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 4)
    ;
  else
  {
    qDebug("UTIL::COMP: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  PlotLine *input2 = 0;
  if (! d.count())
  {
    qDebug("UTIL::COMP: invalid ARRAY_INPUT parm");
    return 0;
  }
  else
    input = d.at(0);

  double inputNum = 0;
  if (d.count() == 1)
  {
    bool ok;
    double t = l[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
    {
      qDebug("UTIL::COMP: invalid DOUBLE parm");
      return 0;
    }
  }
  else
    input2 = d.at(1);

  if (opList.findIndex(l[3]) == -1)
  {
    qDebug("UTIL::COMP: invalid METHOD parm:%s", l[3].latin1());
    return 0;
  }

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;

  PlotLine *line = new PlotLine;

  Operator op = getOperator(l[3]);

  while (loop > -1)
  {
    double t = 0;

    if (! input2)
      t = inputNum;
    else
      if (loop2 > -1)
        t = input2->getData(loop2);
      else
        break;

    switch (op)
    {
      case Equal:
        if (input->getData(loop) == t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      case LessThan:
        if (input->getData(loop) < t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      case LessThanEqual:
        if (input->getData(loop) <= t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      case GreaterThan:
        if (input->getData(loop) > t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      case GreaterThanEqual:
        if (input->getData(loop) >= t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      case And:
        if (input->getData(loop) && t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      case Or:
        if (input->getData(loop) || t)
          line->prepend(1);
        else
          line->prepend(0);
        break;
      default:
        break;
    }

    loop--;

    if (input2)
      loop2--;
  }

  return line;
}

PlotLine * UTIL::calculateCOUNTER (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  formatList.clear();
  QStringList l = QStringList::split(",", p, FALSE);
  if (l.count() == 2)
  {
    formatList.append(FormatString);
    formatList.append(FormatInputArray);
  }
  else
  {
    if (l.count() == 2)
    {
      formatList.append(FormatString);
      formatList.append(FormatInputArray);
      formatList.append(FormatInputArray2);
    }
    else
    {
      qDebug("UTIL::COUNTER: invalid parm count");
      return 0;
    }
  }

  if (checkFormat(p, d, 3, 2))
    return 0;

  PlotLine *in = d.at(0);
  PlotLine *in2 = 0;
  if (d.count() == 2)
    in2 = d.at(1);

  int inLoop = 0;
  int in2Loop = 0;

  if (in2)
  {
    in2Loop = in2->getSize() - in->getSize();
    if (in2Loop < 0)
    {
      inLoop=-in2Loop;
      in2Loop = 0;
    }
  }

  int t = 0;
  PlotLine *counter = new PlotLine;

  while (inLoop < in->getSize())
  {
    if (in2)
    {
      if (in2->getData(in2Loop))
        t = 0;
      if (in->getData(inLoop))
        t++;
      in2Loop++;
    }
    else
    {
      if (in->getData(inLoop))
        t = 1;
      else
        t++;
    }

    counter->append(t);
    inLoop++;
  }

  return counter;
}

PlotLine * UTIL::calculateHL (QString &p, Q3PtrList<PlotLine> &d, int type)
{
  // format1: METHOD, ARRAY_INPUT, DOUBLE
  // format: METHOD, ARRAY_INPUT, ARRAY_INPUT2

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);
  formatList.append(FormatString); // dummy cause we dont know what this parm is beforehand

  if (checkFormat(p, d, 3, 3))
    return 0;

  PlotLine *input = d.at(0);
  PlotLine *input2 = 0;
  if (d.count() == 2)
    input2 = d.at(1);

  double inputNum = 0;
  if (! input2)
  {
    bool ok;
    double t = formatStringList[2].toDouble(&ok);
    if (ok)
      inputNum = t;
    else
      return 0;
  }

  int loop = input->getSize() - 1;
  int loop2 = 0;
  if (input2)
    loop2 = input2->getSize() - 1;

  PlotLine *line = new PlotLine;

  while (loop > -1)
  {
    double v = input->getData(loop);

    if (input2)
    {
      if (loop2 < 0)
        break;

      switch (type)
      {
        case 1: // higher
          if (v < input2->getData(loop2))
            v = input2->getData(loop2);
          break;
        case 2: // lower
          if (v > input2->getData(loop2))
            v = input2->getData(loop2);
          break;
        default:
          break;
      }

      loop2--;
    }
    else
    {
      switch(type)
      {
        case 1:
          if (v < inputNum)
            v = inputNum;
          break;
        case 2:
          if (v > inputNum)
            v = inputNum;
          break;
        default:
          break;
      }
    }

    line->prepend(v);
    loop--;
  }

  return line;
}

PlotLine * UTIL::calculateINRANGE (QString &p, Q3PtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY, DOUBLE, DOUBLE
  // format: METHOD, INPUT_ARRAY, INPUT_ARRAY2, INPUT_ARRAY3

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() != 4)
  {
    qDebug("UTIL::INRANGE: invalid parm count");
    return 0;
  }

  PlotLine *input = 0;
  if (! d.count())
  {
    qDebug("UTIL::INRANGE: invalid ARRAY_INPUT parm");
    return 0;
  }
  input = d.at(0);
  int loop = input->getSize() - 1;

  PlotLine *input2 = 0;
  int loop2 = 0;
  bool ok;
  double min = l[2].toDouble(&ok);
  if (! ok)
  {
    if (d.count() >= 2)
    {
      input2 = d.at(1);
      loop2 = input2->getSize() - 1;
    }
    else
    {
      qDebug("UTIL::INRANGE: invalid MIN parm");
      return 0;
    }
  }

  PlotLine *input3 = 0;
  int loop3 = 0;
  double max = l[3].toDouble(&ok);
  if (! ok)
  {
    if (d.count() == 3)
    {
      input3 = d.at(2);
      loop3 = input3->getSize() - 1;
    }
    else
    {
      qDebug("UTIL::INRANGE: invalid MAX parm");
      return 0;
    }
  }

  PlotLine *line = new PlotLine();
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;

      min = input2->getData(loop2);
      loop2--;
    }

    if (input3)
    {
      if (loop3 < 0)
        break;

      max = input3->getData(loop3);
      loop3--;
    }

    if (input->getData(loop) >= min && input->getData(loop) <= max)
      line->prepend(1);
    else
      line->prepend(0);

    loop--;
  }

  return line;
}

PlotLine * UTIL::calculateNormal(QString &p, Q3PtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);

  if (checkFormat(p, d, 2, 2))
    return 0;

  PlotLine *input = d.at(0);

  PlotLine *normal = new PlotLine;
  int loop = 0;
  double range = 0;
  double max = -99999999.0;
  double min = 99999999.0;
  double norm = 0;
  for (loop = 0; loop < input->getSize(); loop++)
  {
    if (input->getData(loop) > max)
      max = input->getData(loop);

    if (input->getData(loop) < min)
      min = input->getData(loop);
  }

  range = fabs(max) + fabs(min);

  for (loop = 0; loop < input->getSize(); loop++)
  {
    norm = ((input->getData(loop) - min) / range) * 100;
    input->append(norm);
  }

  return normal;
}

PlotLine * UTIL::calculatePER (QString &p, Q3PtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);

  if (checkFormat(p, d, 2, 2))
    return 0;

  PlotLine *line = new PlotLine();
  PlotLine *input = d.at(0);

  double base = input->getData(0);
  int loop;
  for (loop = 1; loop < (int) input->getSize(); loop++)
    line->append(((input->getData(loop) - base) / base) * 100);

  return line;
}

PlotLine * UTIL::calculateREF (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: METHOD, ARRAY_INPUT, PERIOD
  // format2: METHOD, DOUBLE

  PlotLine *line = new PlotLine;

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() < 2 || l.count() > 3)
  {
    qDebug("UTIL::REF: invalid parm count");
    return line;
  }

  if (l.count() == 2)
  {
    bool ok;
    double t = l[1].toDouble(&ok);
    if (! ok)
    {
      qDebug("UTIL::REF: invalid DOUBLE parm");
      return line;
    }

    line->append(t);
  }
  else
  {
    if (! d.count() && l.count() == 3)
    {
      qDebug("UTIL::REF: no input");
      return line;
    }

    bool ok;
    int period;
    int t = l[2].toInt(&ok);
    if (ok)
      period = t;
    else
    {
      qDebug("UTIL::REF: invalid PERIOD parm");
      return line;
    }

    PlotLine *in = d.at(0);

    int loop = 0;
    for (loop = 0; loop < in->getSize(); loop++)
    {
      if (loop - period < 0)
        continue;

      line->append(in->getData(loop - period));
    }
  }

  return line;
}

PlotLine * UTIL::calculateSIGN (QString &p, Q3PtrList<PlotLine> &d)
{
  // format: METHOD, INPUT_ARRAY

  formatList.clear();
  formatList.append(FormatString);
  formatList.append(FormatInputArray);

  if (checkFormat(p, d, 2, 2))
    return 0;

  PlotLine *line = new PlotLine;
  PlotLine *input = d.at(0);

  int loop;
  int sign = 0;
  for (loop = 0; loop < (int) input->getSize(); loop++)
  {
    if (input->getData(loop) > 0)
      sign = 1;
    if (input->getData(loop) < 0)
      sign = -1;
    line->append(sign);
  }

  return line;
}

void UTIL::formatDialog (QStringList &vl, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);

  bool ok;
  QString method = QInputDialog::getItem(QObject::tr("UTIL Indicator Selection"),
                                         QObject::tr("Select an indicator:"),
                                         methodList,
                                         0,
                                         TRUE,
                                         &ok,
                                         0);
  if (! ok)
    return;

  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString ai1l = QObject::tr("Array Input");
  QString ai2l = QObject::tr("Array Input2");
  QString cal = QObject::tr("Color Array");
  QString cil = QObject::tr("Constant Input");
  QString ol = QObject::tr("Operator");
  QString perl = QObject::tr("Period");
  QString cl = QObject::tr("Color");
  QString minl = QObject::tr("Minimum");
  QString maxl = QObject::tr("Maximum");

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("UTIL Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);

  while (1)
  {
     if (! method.compare("ACCUM") || ! method.compare("Normal") || ! method.compare("PER") || ! method.compare("SIGN"))
    {
      // format: METHOD, INPUT_ARRAY
      dialog->addComboItem(ai1l, pl, vl, 0);
      break;
    }

    if (! method.compare("ADD") || ! method.compare("DIV") || ! method.compare("MUL") ||
        ! method.compare("SUB") || ! method.compare("Higher") || ! method.compare("Lower"))
    {
      // format1: METHOD, ARRAY_INPUT, DOUBLE
      // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2
      dialog->addComboItem(ai1l, pl, vl, 0);
      QStringList l = vl;
      l.append(QObject::tr("None"));
      dialog->addComboItem(ai2l, pl, l, l.count() - 1);
      dialog->addDoubleItem(cil, pl, 0, -99999999.0, 99999999.0);
      break;
    }

    if (! method.compare("COMP"))
    {
      // format1: METHOD, ARRAY_INPUT, DOUBLE, OPERATOR
      // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR
      dialog->addComboItem(ai1l, pl, vl, 0);
      QStringList l = vl;
      l.append(QObject::tr("None"));
      dialog->addComboItem(ai2l, pl, l, l.count() - 1);
      dialog->addDoubleItem(cil, pl, 0, -99999999.0, 99999999.0);
      dialog->addComboItem(ol, pl, opList, 0);
      break;
    }

    if (! method.compare("COUNTER"))
    {
      // format1: METHOD, ARRAY_INPUT
      // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2
      dialog->addComboItem(ai1l, pl, vl, 0);
      QStringList l = vl;
      l.append(QObject::tr("None"));
      dialog->addComboItem(ai2l, pl, l, l.count() - 1);
      break;
    }

    if (! method.compare("REF"))
    {
      // format1: METHOD, ARRAY_INPUT, PERIOD
      // format2: METHOD, DOUBLE
      QStringList l = vl;
      l.append(QObject::tr("None"));
      dialog->addComboItem(ai1l, pl, l, l.count() - 1);
      dialog->addDoubleItem(cil, pl, 0, -99999999.0, 99999999.0);
      dialog->addIntItem(perl, pl, 1, 0, 999999);
      break;
    }

    if (! method.compare("COLOR"))
    {
      // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR
      dialog->addComboItem(ai1l, pl, vl, 0);
      dialog->addComboItem(cal, pl, vl, 0);
      dialog->addDoubleItem(cil, pl, 0, -99999999.0, 99999999.0);
      QColor color("red");
      dialog->addColorItem(cl, pl, color);
      break;
    }

    if (! method.compare("INRANGE"))
    {
      // format1: METHOD, ARRAY_INPUT, DOUBLE, DOUBLE
      // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, ARRAY_INPUT3
      dialog->addComboItem(ai1l, pl, vl, 0);
      QStringList l = vl;
      l.append(QObject::tr("None"));
      QString s = QObject::tr("Min Array");
      dialog->addComboItem(s, pl, l, l.count() - 1);
      dialog->addDoubleItem(minl, pl, 0, -99999999.0, 99999999.0);
      s = QObject::tr("Max Array");
      dialog->addComboItem(s, pl, l, l.count() - 1);
      dialog->addDoubleItem(maxl, pl, 0, -99999999.0, 99999999.0);
      break;
    }

    break;
  }

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    rs = method;

    while (1)
    {
      if (! method.compare("ACCUM") || ! method.compare("Normal") || ! method.compare("PER") || ! method.compare("SIGN"))
      {
        // format: METHOD, INPUT_ARRAY
        dialog->getCombo(ai1l, s);
        rs.append("," + s);
        break;
      }

      if (! method.compare("ADD") || ! method.compare("DIV") || ! method.compare("MUL") ||
          ! method.compare("SUB") || ! method.compare("Higher") || ! method.compare("Lower"))
      {
        // format1: METHOD, ARRAY_INPUT, DOUBLE
        // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2
        dialog->getCombo(ai1l, s);
        rs.append("," + s);

        dialog->getCombo(ai2l, s);
        if (! s.compare(QObject::tr("None")))
        {
          double d = dialog->getDouble(cil);
          rs.append("," + QString::number(d));
        }
        else
          rs.append("," + s);
        break;
      }

      if (! method.compare("COMP"))
      {
        // format1: METHOD, ARRAY_INPUT, DOUBLE, OPERATOR
        // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR
        dialog->getCombo(ai1l, s);
        rs.append("," + s);

        dialog->getCombo(ai2l, s);
        if (! s.compare(QObject::tr("None")))
        {
          double d = dialog->getDouble(cil);
          rs.append("," + QString::number(d));
        }
        else
          rs.append("," + s);

        dialog->getCombo(ol, s);
        rs.append("," + s);
        break;
      }

      if (! method.compare("COUNTER"))
      {
        // format1: METHOD, ARRAY_INPUT
        // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2
        dialog->getCombo(ai1l, s);
        rs.append("," + s);

        dialog->getCombo(ai2l, s);
        if (s.compare(QObject::tr("None")))
          rs.append("," + s);
        break;
      }

      if (! method.compare("REF"))
      {
        // format1: METHOD, ARRAY_INPUT, PERIOD
        // format2: METHOD, DOUBLE
        dialog->getCombo(ai1l, s);
        if (! s.compare(QObject::tr("None")))
        {
          double d = dialog->getDouble(cil);
          rs.append("," + QString::number(d));
        }
        else
        {
          rs.append("," + s);

          int t = dialog->getInt(perl);
          rs.append("," + QString::number(t));
        }
        break;
      }

      if (! method.compare("COLOR"))
      {
        // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR
        dialog->getCombo(ai1l, s);
        rs.append("," + s);

        dialog->getCombo(cal, s);
        rs.append("," + s);

        double d = dialog->getDouble(cil);
        rs.append("," + QString::number(d));

        QColor color;
        dialog->getColor(cl, color);
        rs.append("," + color.name());
        break;
      }

      if (! method.compare("INRANGE"))
      {
        // format1: METHOD, ARRAY_INPUT, DOUBLE, DOUBLE
        dialog->getCombo(ai1l, s);

        QString s = QObject::tr("Min Array");
        QString s2;
        dialog->getCombo(s, s2);
        if (! s2.compare(QObject::tr("None")))
        {
          double d = dialog->getDouble(minl);
          rs.append("," + QString::number(d));
        }
        else
          rs.append("," + s2);

        s = QObject::tr("Max Array");
        dialog->getCombo(s, s2);
        if (! s2.compare(QObject::tr("None")))
        {
          double d = dialog->getDouble(maxl);
          rs.append("," + QString::number(d));
        }
        else
          rs.append("," + s2);

        break;
      }

      break;
    }
  }

  delete dialog;
}
