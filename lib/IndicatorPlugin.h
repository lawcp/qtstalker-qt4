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

#ifndef INDICATORPLUGIN_HPP
#define INDICATORPLUGIN_HPP

#include "PlotLine.h"
#include "Setting.h"
#include "BarData.h"
#include "Indicator.h"
#include <qstring.h>
#include <qstringlist.h>
#include <q3dict.h>
#include <q3valuelist.h>
#include <qobject.h>
//Added by qt3to4:
#include <Q3PtrList>

class IndicatorPlugin : public QObject
{
  Q_OBJECT

  signals:
    void signalWakeup();

  public:

    enum FormatType
    {
      FormatInputArray,
      FormatInputArray2,
      FormatInteger,
      FormatDouble,
      FormatString,
      FormatMAType,
      FormatBool
    };

    enum Operator
    {
      Equal,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,
      And,
      Or
    };

    IndicatorPlugin();
    virtual ~IndicatorPlugin();
    void setIndicatorInput(BarData *);
    void loadFile(QString &, Setting &);
    void saveFile(QString &, Setting &);
    void getPluginName(QString &);
    void getHelpFile(QString &);
    IndicatorPlugin::Operator getOperator(QString &);
    PlotLine * getWilderMA(PlotLine *d, int);
    void getMATypes(QStringList &);
    bool checkFormat(QString &, Q3PtrList<PlotLine> &, int, int);
    void setFormatMethod(QString &);

    virtual Indicator * calculate();
    virtual int indicatorPrefDialog(QWidget *);
    virtual PlotLine * calculateCustom(QString &, Q3PtrList<PlotLine> &);
    virtual void getIndicatorSettings(Setting &);
    virtual void setIndicatorSettings(Setting &);
    virtual void setCustomFunction(QStringList &);
    virtual void loadIndicatorSettings(QString &);
    virtual void saveIndicatorSettings(QString &);
    virtual PlotLine * getMA(PlotLine *d, int, int);
    virtual void formatDialog(QStringList &, QString &, QString &);

  public slots:
    void wakeup();

  protected:
    BarData *data;
    QStringList lineTypes;
    QStringList inputTypeList;
    QStringList opList;
    QStringList maList;
    bool saveFlag;
    QString pluginName;
    QString helpFile;
    Q3ValueList<FormatType> formatList;
    QStringList formatStringList;
    bool dateFlag;
    bool logScale;
    QString formatMethod;
};

#endif
