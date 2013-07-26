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

#ifndef COBASE_HPP
#define COBASE_HPP

#include "Scaler.h"
#include "BarData.h"
#include "Setting.h"
#include <qpixmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qmenu.h>
#include <qpoint.h>
#include <qregion.h>
#include <qcolor.h>
#include <q3ptrlist.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QKeyEvent>

#define HANDLE_WIDTH 6

class COBase : public QObject
{
  Q_OBJECT

  signals:
    void signalDraw();
    void signalRefresh();
    void message(QString);
    void signalObjectDeleted(QString);
    void signalSave(QString);

  public:
    enum Status
    {
      None,
      ClickWait,
      ClickWait2,
      Selected,
      Moving
    };

    COBase();
    virtual ~COBase();
    virtual void draw(QPixmap &, Scaler &, int, int, int);
    virtual void newObject(QString &, QString &);
    virtual void keyEvent(QKeyEvent *);
    virtual double getHigh();
    virtual double getLow();
    virtual void getSettings(Setting &);
    virtual void setSettings(Setting &);
    virtual void adjustForSplit(QDateTime &, double);

    void setData(BarData *);
    QString getHelpFile();
    void setSaveFlag(bool);
    bool getSaveFlag();
    void clearGrabHandles();
    void setGrabHandle(QRegion *);
    void clearSelectionArea();
    void setSelectionArea(QRegion *);
    QString getName();
    void setDate(QDateTime &);
    void getDate(QDateTime &);
    void setColor(QColor);
    QColor getColor();
    void setValue(double);
    double getValue();
    void setStatus(Status);
    COBase::Status getStatus();
    bool isSelected(QPoint point);
    bool isGrabSelected(QPoint point);
    COBase * getCO(Setting &);
    COBase * getCO(QString &);

  public slots:
    virtual void prefDialog();
    virtual COBase::Status pointerClick(QPoint &, QDateTime &, double);
    virtual void pointerMoving(QPixmap &, QPoint &, QDateTime &, double y);

    void removeObject();
    void moveObject();
    void addObject(Setting &);
    void showMenu();

  protected:
    BarData *data;
    QMenu *menu;
    QString indicator;
    QString name;
    QString helpFile;
    QString plot;
    Status status;
    QColor defaultColor;
    bool saveFlag;
    Q3PtrList<QRegion> grabHandles;
    Q3PtrList<QRegion> selectionArea;
    QColor color;
    QDateTime date;
    double value;
    QString type;
    QString dateFormat;
    QString dateLabel;
    QString valueLabel;
    QString colorLabel;
    QString plotLabel;
    QString nameLabel;
    QString typeLabel;
    QString identifier;
    QString identifierLabel;
    QString price;
    QString priceLabel;
};

#endif
