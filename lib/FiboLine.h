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

#ifndef FIBOLINE_HPP
#define FIBOLINE_HPP

#include "COBase.h"
#include "Setting.h"
#include <q3pointarray.h>
#include <q3dict.h>
#include <qpoint.h>
#include <qfont.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QPixmap>

class FiboLine : public COBase
{
  Q_OBJECT

  public:
    FiboLine();
    ~FiboLine();
    void draw(QPixmap &, Scaler &, int, int, int);
    void newObject(QString &, QString &);
    double getHigh();
    double getLow();
    void loadDefaults();
    void saveDefaults();
    double getY(double v, double high, double low);
    void getSettings(Setting &);
    void setSettings(Setting &);
    int isGrabSelected(QPoint point);
    double getLine(int);
    void adjustForSplit(QDateTime &, double);

  public slots:
    void prefDialog();
    COBase::Status pointerClick(QPoint &, QDateTime &, double);
    void pointerMoving(QPixmap &, QPoint &, QDateTime &, double);

  protected:
    int moveFlag;
    QDateTime tx;
    double ty;
    int mpx, mpx2, mpy, mpy2;
    QFont font;
    double line1;
    double line2;
    double line3;
    double line4;
    double line5;
    double line6;
    QDateTime startDate;
    QDateTime endDate;
    double high;
    double low;
    bool extend;
    QString highLabel;
    QString lowLabel;
    QString sdateLabel;
    QString edateLabel;
    QString extendLabel;
    QString l1Label;
    QString l2Label;
    QString l3Label;
    QString l4Label;
    QString l5Label;
    QString l6Label;
};

#endif
