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

#ifndef SCALER_HPP
#define SCALER_HPP

#include <qstringlist.h>
#include <QVector>

class Scaler
{
  public:
    Scaler();
    ~Scaler();
    void set(int ht, double h, double l, double lh, double lr, bool lf);
    int convertToY(double);
    double convertToVal(int);
    void getScaleArray(QVector<double> &);
    double getLogScaleHigh();
    double getLogRange();
    int getHeight();
    bool getDateFlag();
    bool getLogFlag();
    double getLow();

  protected:
    int height;
    bool logScale;
    double scaleHigh;
    double scaleLow;
    double logScaleHigh;
    double logRange;
    double range;
    double scaler;
    QStringList scaleList;
};

#endif
