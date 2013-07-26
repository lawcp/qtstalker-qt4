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

#ifndef TRENDLINE_HPP
#define TRENDLINE_HPP

#include "COBase.h"
#include "Setting.h"
#include <q3pointarray.h>
#include <qpoint.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QPixmap>

class TrendLine : public COBase
{
  Q_OBJECT

  public:
    TrendLine ();
    ~TrendLine ();
    void draw (QPixmap &, Scaler &, int, int, int);
    void newObject (QString &, QString &);
    double getHigh ();
    double getLow ();
    void drawMovingPointer (QPixmap &, QPoint &);
    void loadDefaults ();
    void saveDefaults ();
    void getSettings (Setting &);
    void setSettings (Setting &);
    void setDate2 (QDateTime &);
    void getDate2 (QDateTime &);
    void setValue2 (double);
    double getValue2 ();
    int isGrabSelected (QPoint point);
    bool getUseBar ();
    void getBar (QString &);
    bool getExtend ();
    void adjustForSplit (QDateTime &, double);

  public slots:
    void prefDialog ();
    COBase::Status pointerClick (QPoint &, QDateTime &, double);
    void pointerMoving (QPixmap &, QPoint &, QDateTime &, double);

  protected:
    bool usebar;
    bool extend;
    QString bar;
    int moveFlag;
    QDateTime tx;
    double ty;
    int mpx, mpx2, mpy, mpy2;
    QDateTime date2;
    double value2;
    QString sdateLabel;
    QString edateLabel;
    QString svalueLabel;
    QString evalueLabel;
    QString fieldLabel;
    QString usebarLabel;
    QString extendLabel;

};

#endif
