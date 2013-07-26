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

#include "TALIB.h"
#include "PrefDialog.h"
#include <q3dict.h>
#include <qobject.h>
#include <qinputdialog.h>
//Added by qt3to4:
#include <Q3PtrList>

TALIB::TALIB ()
{
  pluginName = "TALIB";
  helpFile = "talib.html";

  setDefaults();
}

TALIB::~TALIB ()
{
  TA_Shutdown();
}

void TALIB::setDefaults ()
{
  getIndicatorList(methodList);

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");
}

Indicator * TALIB::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  // open a TALIB handle
  const TA_FuncHandle *handle;
  QString ts = "method";
  QString ts2;
  parms.getData(ts, ts2);
  TA_RetCode retCode = TA_GetFuncHandle(ts2, &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculate:can't open handle");
    return output;
  }

  // get info on the function
  const TA_FuncInfo *theInfo;
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculate:can't get function info");
    return output;
  }

  // create parm holder
  TA_ParamHolder *parmHolder;
  retCode = TA_ParamHolderAlloc(handle, &parmHolder);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculate:can't create parm holder");
    return output;
  }

  // create and populate the input arrays
  int loop = data->count();
  TA_Real open[loop];
  TA_Real high[loop];
  TA_Real low[loop];
  TA_Real close[loop];
  TA_Real volume[loop];
  TA_Real oi[loop];
  for (loop = 0; loop < data->count(); loop++)
  {
    open[loop] = (TA_Real) data->getOpen(loop);
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
    oi[loop] = (TA_Real) data->getOI(loop);
  }

  // setup the input arrays
  const TA_InputParameterInfo *inputParms;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    QString s;
    int tint = 3;
    switch (inputParms->type)
    {
      case TA_Input_Price:
        retCode = TA_SetInputParamPricePtr(parmHolder, loop, &open[0], &high[0], &low[0],
                                           &close[0], &volume[0], &oi[0]);
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set input prices");
        break;
      case TA_Input_Real:
        s = QObject::tr("input") + QString::number(loop + 1);
        tint = parms.getInt(s);
        switch (tint)
        {
          case 0:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &open[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 1:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &high[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 2:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &low[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 3:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &close[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 4:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &volume[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set integer price");
            break;
          case 5:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &oi[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set integer price");
            break;
          default:
            break;
        }
        break;
      case TA_Input_Integer:
        break;
      default:
        break;
    }
  }

  // setup the optinput parms
  const TA_OptInputParameterInfo *optInfo;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    QString s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        retCode = TA_SetOptInputParamReal(parmHolder, loop, (TA_Real) parms.getDouble(s));
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set inputopt real");
        break;
      case TA_OptInput_IntegerRange:
        retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) parms.getInt(s));
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set inputopt integer");
        break;
      case TA_OptInput_IntegerList:
        retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) parms.getInt(s));
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set inputopt integerlist");
        break;
      default:
        break;
    }
  }

  // setup the output arrays
  TA_Real out1[data->count()];
  TA_Real out2[data->count()];
  TA_Real out3[data->count()];
  TA_Integer out4[data->count()];
  const TA_OutputParameterInfo *outInfo;
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++)
  {
    retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("TALIB::calculate:cannot get output info");
      continue;
    }

    switch (loop)
    {
      case 0:
        if (outInfo->type == TA_Output_Integer)
        {
          retCode = TA_SetOutputParamIntegerPtr(parmHolder, loop, &out4[0]);
          if (retCode != TA_SUCCESS)
            qDebug("TALIB::calculate:cannot set output4");
        }
        else
        {
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out1[0]);
          if (retCode != TA_SUCCESS)
            qDebug("TALIB::calculate:cannot set output1");
        }
        break;
      case 1:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out2[0]);
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set output2");
        break;
      case 2:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out3[0]);
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set output3");
        break;
      default:
        break;
    }
  }

  // call the function
  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  TA_Integer outstart;
  TA_Integer count;
  retCode = TA_CallFunc(parmHolder, start, end, &outstart, &count);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculate:call function failed");
  else
  {
    // create the plotlines
    const TA_OutputParameterInfo *outInfo;
    for (loop = 0; loop < (int) theInfo->nbOutput; loop++ )
    {
      TA_GetOutputParameterInfo(theInfo->handle, loop, &outInfo);
      QString base = outInfo->paramName;
      base = base.right(base.length() - 3);
      if (! base.left(4).compare("Real"))
        base = base.right(base.length() - 4);
      if (! base.left(7).compare("Integer"))
        base = base.right(base.length() - 7);
      if (! base.length())
        base = QObject::tr("Plot");

      PlotLine *line = new PlotLine;

      QString s = base + " " + QObject::tr("Color");
      parms.getData(s, ts);
      QColor color(ts);
      line->setColor(color);

      s = base + " " + QObject::tr("Label");
      parms.getData(s, ts);
      line->setLabel(ts);

      s = base + " " + QObject::tr("Line Type");
      line->setType((PlotLine::LineType)parms.getInt(s));

      retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
      if (retCode != TA_SUCCESS)
      {
        qDebug("TALIB::calculate:cannot get output info");
        delete line;
        continue;
      }

      int loop2;
      switch (loop)
      {
        case 0:
          if (outInfo->type == TA_Output_Integer)
          {
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out4[loop2]);
          }
          else
          {
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out1[loop2]);
          }
          break;
        case 1:
          for (loop2 = 0; loop2 < count; loop2++)
            line->append((double) out2[loop2]);
          break;
        case 2:
          for (loop2 = 0; loop2 < count; loop2++)
            line->append((double) out3[loop2]);
          break;
        default:
          break;
      }

      if (line->getType() == PlotLine::Histogram || line->getType() == PlotLine::HistogramBar)
        output->prependLine(line);
      else
        output->addLine(line);
    }
  }

  retCode = TA_ParamHolderFree(parmHolder);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculate:can't delete parm holder");

  return output;
}

int TALIB::indicatorPrefDialog (QWidget *w)
{
  const TA_FuncHandle *handle;
  const TA_FuncInfo *theInfo;

  // open a TALIB handle
  QString ts = "method";
  QString ts2;
  parms.getData(ts, ts2);
  TA_RetCode retCode = TA_GetFuncHandle(ts2, &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::indicatorPrefDialog:can't open handle");
    return FALSE;
  }

  // get info on the function
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::indicatorPrefDialog:can't get function info");
    return FALSE;
  }

  QString pl = QObject::tr("Parms");
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("TALIB Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QStringList mal;
  getMATypes(mal);
  mal.remove("Wilder");

  // get the input parms
  const TA_OptInputParameterInfo *optInfo;
  int loop;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    QString s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        parms.getData(s, ts);
        if (! ts.length())
        {
          dialog->addDoubleItem(s, pl, (double) optInfo->defaultValue, 0, 99999999);
          ts = QString::number((double) optInfo->defaultValue);
          parms.setData(s, ts);
        }
        else
          dialog->addDoubleItem(s, pl, parms.getDouble(s), 0, 99999999);
        break;
      case TA_OptInput_IntegerRange:
        parms.getData(s, ts);
        if (! ts.length())
        {
          dialog->addIntItem(s, pl, (int) optInfo->defaultValue, 1, 999999);
          ts = QString::number((int) optInfo->defaultValue);
          parms.setData(s, ts);
        }
        else
          dialog->addIntItem(s, pl, parms.getInt(s), 1, 999999);
        break;
      case TA_OptInput_IntegerList:
        parms.getData(s, ts);
        if (! ts.length())
        {
          dialog->addComboItem(s, pl, mal, (int) optInfo->defaultValue);
          ts = QString::number((int) optInfo->defaultValue);
          parms.setData(s, ts);
        }
        else
          dialog->addComboItem(s, pl, mal, parms.getInt(s));
        break;
      case TA_OptInput_RealList:
        break;
      default:
        break;
    }
  }

  // check for any array inputs
  const TA_InputParameterInfo *inputParms;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    QString s = QObject::tr("input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        parms.getData(s, ts);
        if (! ts.length())
        {
          dialog->addComboItem(s, pl, inputTypeList, (int) BarData::Close);
          ts = QString::number(BarData::Close);
          parms.setData(s, ts);
        }
        else
          dialog->addComboItem(s, pl, inputTypeList, parms.getInt(s));
        break;
      default:
        break;
    }
  }

  // setup the output plots
  const TA_OutputParameterInfo *outInfo;
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++ )
  {
    TA_GetOutputParameterInfo(theInfo->handle, loop, &outInfo);

    pl = outInfo->paramName;
    pl = pl.right(pl.length() - 3);
    if (! pl.left(4).compare("Real"))
      pl = pl.right(pl.length() - 4);
    if (! pl.left(7).compare("Integer"))
      pl = pl.right(pl.length() - 7);
    if (! pl.length())
      pl = QObject::tr("Plot");
    dialog->createPage (pl);

    QString s = pl + " " + QObject::tr("Color");
    QColor color;
    if (loop == 0)
      color.setNamedColor("red");
    else
    {
      if (loop == 1)
        color.setNamedColor("yellow");
      else
        color.setNamedColor("blue");
    }
    parms.getData(s, ts);
    if (! ts.length())
    {
      dialog->addColorItem(s, pl, color);
      ts = color.name();
      parms.setData(s, ts);
    }
    else
    {
      parms.getData(s, ts);
      color.setNamedColor(ts);
      dialog->addColorItem(s, pl, color);
    }

    s = pl + " " + QObject::tr("Label");
    parms.getData(s, ts);
    if (! ts.length())
    {
      dialog->addTextItem(s, pl, pl);
      parms.setData(s, pl);
    }
    else
    {
      parms.getData(s, ts);
      dialog->addTextItem(s, pl, ts);
    }

    s = pl + " " + QObject::tr("Line Type");
    parms.getData(s, ts);
    if (! ts.length())
    {
      switch (outInfo->flags)
      {
        case TA_OUT_DOT_LINE:
          ts = QString::number(PlotLine::Dot);
          break;
        case TA_OUT_DASH_LINE:
          ts = QString::number(PlotLine::Dash);
          break;
        case TA_OUT_HISTO:
          ts = QString::number(PlotLine::Histogram);
          break;
        default:
          ts = QString::number(PlotLine::Line);
          break;
      }

      dialog->addComboItem(s, pl, lineTypes, ts.toInt());
      parms.setData(s, ts);
    }
    else
      dialog->addComboItem(s, pl, lineTypes, parms.getInt(s));
  }

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList l;
    parms.getKeyList(l);
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QString s;
      dialog->getItem(l[loop], s);
      if (s.length())
        parms.setData(l[loop], s);
    }

    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void TALIB::getIndicatorSettings (Setting &dict)
{
  QString s;
  parms.getString(s);
  dict.parse(s);
  s = "plugin";
  dict.setData(s, pluginName);
}

void TALIB::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getString(s);
  parms.parse(s);
}

void TALIB::getIndicatorList (QStringList &l)
{
  l.clear();

  TA_StringTable *table;
  int loop;
  QStringList cl;

  TA_RetCode retCode = TA_GroupTableAlloc(&table);
  if (retCode == TA_SUCCESS)
  {
    for (loop = 0; loop < (int) table->size; loop++)
      cl.append(table->string[loop]);

    TA_GroupTableFree(table);
  }

  for (loop = 0; loop < (int) cl.count(); loop++)
  {
    retCode = TA_FuncTableAlloc(cl[loop], &table);
    if (retCode == TA_SUCCESS)
    {
      int loop2;
      for (loop2 = 0; loop2 < (int) table->size; loop2++ )
        l.append(table->string[loop2]);

      TA_FuncTableFree(table);
    }
  }

  l.sort();
}

PlotLine * TALIB::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format: METHOD, ..., ...., ..... etc (first parm must be the method)

  QStringList l = QStringList::split(",", p, FALSE);

  if (! l.count())
  {
    qDebug("TALIB::calculateCustom: no method parm");
    return 0;
  }

  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  if (d.count())
    end = d.at(0)->getSize() - 1;
  TA_Integer outstart;
  TA_Integer count;
  PlotLine *line = new PlotLine;
  // sometimes are not enough data available
  // to calc anything
  if(end < 0)
    return line;

  // open a TALIB handle
  const TA_FuncHandle *handle;
  TA_RetCode retCode = TA_GetFuncHandle(l[0], &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculateCustom:can't open handle");
    return 0;
  }

  // get info on the function
  const TA_FuncInfo *theInfo;
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculateCustom:can't get function info");
    return 0;
  }

  // create parm holder
  TA_ParamHolder *parmHolder;
  retCode = TA_ParamHolderAlloc(handle, &parmHolder);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculateCustom:can't create parm holder");
    return 0;
  }

  // create and input arrays
  int loop = data->count();
  TA_Real open[loop];
  TA_Real high[loop];
  TA_Real low[loop];
  TA_Real close[loop];
  TA_Real volume[loop];
  TA_Real oi[loop];
  TA_Real treal[loop];

  int sparmIndex = 1;

  // setup the input arrays
  const TA_InputParameterInfo *inputParms;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);

    if (inputParms->type == TA_Input_Price)
    {
      // populate the input arrays
      int loop2;
      for (loop2 = 0; loop2 < data->count(); loop2++)
      {
        open[loop2] = (TA_Real) data->getOpen(loop2);
        high[loop2] = (TA_Real) data->getHigh(loop2);
        low[loop2] = (TA_Real) data->getLow(loop2);
        close[loop2] = (TA_Real) data->getClose(loop2);
        volume[loop2] = (TA_Real) data->getVolume(loop2);
        oi[loop2] = (TA_Real) data->getOI(loop2);
      }

      retCode = TA_SetInputParamPricePtr(parmHolder, loop, &open[0], &high[0], &low[0], &close[0],
                                         &volume[0], &oi[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug("TALIB::calculateCustom:cannot set input prices");
        return 0;
      }
    }

    if (inputParms->type == TA_Input_Real)
    {
      if (! d.count())
      {
        qDebug("TALIB::calculateCustom: no input");
        return 0;
      }

      if (sparmIndex >= (int) l.count())
      {
        qDebug("TALIB::calculateCustom: input invalid number of parms");
        return 0;
      }

      PlotLine *line =  d.at(0);
      int loop2;
      for (loop2 = 0; loop2 < line->getSize(); loop2++)
        treal[loop2] = (TA_Real) line->getData(loop2);

      retCode = TA_SetInputParamRealPtr(parmHolder, loop, &treal[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug("TALIB::calculateCustom:cannot set real price");
        return 0;
      }

      sparmIndex++;
    }
  }

  if (sparmIndex < (int) l.count())
  {
    QStringList mal;
    getMATypes(mal);
    mal.remove("Wilder");
    int t = 0;

    // setup the optinput parms
    const TA_OptInputParameterInfo *optInfo;
    for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
    {
      TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
      switch (optInfo->type)
      {
        case TA_OptInput_RealRange:
          if (sparmIndex >= (int) l.count())
          {
            qDebug("TALIB::calculateCustom: optinput real invalid number of parms");
            return 0;
          }

          retCode = TA_SetOptInputParamReal(parmHolder, loop, (TA_Real) l[sparmIndex].toDouble());
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set inputopt real");
            return 0;
          }
          sparmIndex++;
          break;
        case TA_OptInput_IntegerRange:
          if (sparmIndex >= (int) l.count())
          {
            qDebug("TALIB::calculateCustom: optinput integer invalid number of parms");
            return 0;
          }

          retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) l[sparmIndex].toInt());
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set inputopt integer");
            return 0;
          }
          sparmIndex++;
          break;

        case TA_OptInput_IntegerList:
          if (sparmIndex >= (int) l.count())
          {
            qDebug("TALIB::calculateCustom: optinput integerList invalid number of parms");
            return 0;
          }

          t = mal.findIndex(l[sparmIndex]);
          if (t == -1)
            t = 0;
          retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) t);
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set inputopt integer");
            return 0;
          }
          sparmIndex++;
          break;
        default:
          break;
      }
    }
  }

  // setup the output arrays
  TA_Real out1[data->count()];
  TA_Real out2[data->count()];
  TA_Real out3[data->count()];
  TA_Integer out4[data->count()];
  const TA_OutputParameterInfo *outInfo;
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++)
  {
    retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("TALIB::calculateCustom:cannot get output info");
      return 0;
    }

    switch (loop)
    {
      case 0:
        if (outInfo->type == TA_Output_Integer)
        {
          retCode = TA_SetOutputParamIntegerPtr(parmHolder, loop, &out4[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set output4");
            return 0;
          }
        }
        else
        {
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out1[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set output1");
            return 0;
          }
        }
        break;
      case 1:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out2[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("TALIB::calculateCustom:cannot set output2");
          return 0;
        }
        break;
      case 2:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out3[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("TALIB::calculateCustom:cannot set output3");
          return 0;
        }
        break;
      default:
        break;
    }
  }

  // call the function
  /*
  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  if (d.count())
    end = d.at(0)->getSize() - 1;
  TA_Integer outstart;
  TA_Integer count;
  PlotLine *line = new PlotLine;
  */
  retCode = TA_CallFunc(parmHolder, start, end, &outstart, &count);
  if (retCode != TA_SUCCESS)
  {
    printError(QString("TALIB::calculateCustom:TA_CallFunc"), retCode);
    qDebug("p=%s  start=%d  end=%d",p.ascii(), start, end);
  }
  else
  {
    // create the plotlines
    int loop2;
    retCode = TA_GetOutputParameterInfo(handle, 0, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("TALIB::calculateCustom:cannot get output info");
      return 0;
    }

    if (outInfo->type == TA_Output_Integer)
    {
      for (loop2 = 0; loop2 < count; loop2++)
        line->append((double) out4[loop2]);
    }
    else
    {
      if (theInfo->nbOutput > 1)
      {
        bool ok;
        l[l.count() - 1].toInt(&ok);
        if (! ok)
        {
          qDebug("TALIB::calculateCustom: parm #%i invalid, not an INTEGER", loop + 1);
          return 0;
        }

        switch (l[l.count() - 1].toInt(&ok))
        {
          case 2:
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out2[loop2]);
            break;
          case 3:
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out3[loop2]);
            break;
          default:
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out1[loop2]);
            break;
        }
      }
      else
      {
        for (loop2 = 0; loop2 < count; loop2++)
          line->append((double) out1[loop2]);
      }
    }
  }

  retCode = TA_ParamHolderFree(parmHolder);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculateCustom:can't delete parm holder");

  return line;
}

PlotLine * TALIB::getMA (PlotLine *in, int type, int period)
{
  PlotLine *ma = new PlotLine;

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Integer outBeg;
  TA_Integer count;
  TA_RetCode rc = TA_SUCCESS;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  switch (type)
  {
    case 0:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_SMA, &outBeg, &count, &out[0]);
      break;
    case 1:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_EMA, &outBeg, &count, &out[0]);
      break;
    case 2:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_WMA, &outBeg, &count, &out[0]);
      break;
    case 3:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_DEMA, &outBeg, &count, &out[0]);
      break;
    case 4:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_TEMA, &outBeg, &count, &out[0]);
      break;
    case 5:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_TRIMA, &outBeg, &count, &out[0]);
      break;
    case 6:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_KAMA, &outBeg, &count, &out[0]);
      break;
    case 7:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_MAMA, &outBeg, &count, &out[0]);
      break;
    case 8:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_T3, &outBeg, &count, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug("TALIB::getMA:error on TALIB function call");
    return ma;
  }

  for (loop = 0; loop < count; loop++)
    ma->append((double) out[loop]);

  return ma;
}

void TALIB::formatDialog (QStringList &vl, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);

  const TA_FuncHandle *handle;
  const TA_FuncInfo *theInfo;

  // open a TALIB handle
  TA_RetCode retCode = TA_GetFuncHandle(formatMethod, &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::getFormatList:can't open handle");
    return;
  }

  // get info on the function
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::getFormatList:can't get function info");
    return;
  }

  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("TALIB Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);

  // check for any array inputs
  const TA_InputParameterInfo *inputParms;
  int loop;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    s = QObject::tr("Input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        dialog->addComboItem(s, pl, vl, (int) BarData::Close);
        break;
      default:
        break;
    }
  }

  QStringList mal;
  getMATypes(mal);
  mal.remove("Wilder");

  // get the input parms
  const TA_OptInputParameterInfo *optInfo;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        dialog->addDoubleItem(s, pl, (double) optInfo->defaultValue, 0, 99999999);
        break;
      case TA_OptInput_IntegerRange:
        dialog->addIntItem(s, pl, (int) optInfo->defaultValue, 1, 999999);
        break;
      case TA_OptInput_IntegerList:
        dialog->addComboItem(s, pl, mal, (int) optInfo->defaultValue);
        break;
      case TA_OptInput_RealList:
        break;
      default:
        break;
    }
  }

  if (theInfo->nbOutput > 1)
  {
    s = "Plot";
    dialog->addIntItem(s, pl, 1, 1, theInfo->nbOutput);
  }

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    rs = formatMethod;

    QString ts;
    for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
    {
      s = QObject::tr("Input") + QString::number(loop + 1);
      TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
      switch (inputParms->type)
      {
        case TA_Input_Real:
          dialog->getCombo(s, ts);
          rs.append("," + ts);
          break;
        default:
          break;
      }
    }

    double d = 0;
    int t = 0;
    for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
    {
      TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
      s = optInfo->displayName;
      switch (optInfo->type)
      {
        case TA_OptInput_RealRange:
          d = dialog->getDouble(s);
          rs.append("," + QString::number(d));
          break;
        case TA_OptInput_IntegerRange:
          t = dialog->getInt(s);
          rs.append("," + QString::number(t));
          break;
        case TA_OptInput_IntegerList:
          dialog->getCombo(s, ts);
          rs.append("," + ts);
          break;
        case TA_OptInput_RealList:
          break;
        default:
          break;
      }
    }

    if (theInfo->nbOutput > 1)
    {
      s = "Plot";
      t = dialog->getInt(s);
      rs.append("," + QString::number(t));
    }
  }

  delete dialog;
}

void TALIB::printError (QString es, TA_RetCode rc)
{
  TA_RetCodeInfo info;
  TA_SetRetCodeInfo(rc, &info);
  qDebug("%s:%d(%s): %s", es.latin1(), rc, info.enumStr, info.infoStr);
}
