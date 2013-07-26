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

#include "TrendLine.h"
#include "PrefDialog.h"
#include "Config.h"
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QPixmap>

TrendLine::TrendLine ()
{
  defaultColor.setNamedColor("red");
  usebar = FALSE;
  extend = TRUE;
  bar = "Close";
  helpFile = "trendline.html";
  date2 = date;
  type = "TrendLine";

  sdateLabel = "Start Date";
  edateLabel = "End Date";
  svalueLabel = "Start Value";
  evalueLabel = "End Value";
  fieldLabel = "Bar Field";
  usebarLabel = "Use Bar";
  extendLabel = "Extend Line";

  loadDefaults();
}

TrendLine::~TrendLine ()
{
}

void TrendLine::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  x2 = data->getX(date2);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;

  int y;
  int y2;
  if (getUseBar())
  {
    int i = data->getX(date);
    int i2 = data->getX(date2);

    while (1)
    {
      if (! bar.compare(tr("Open")))
      {
        y = scaler.convertToY(data->getOpen(i));
        y2 = scaler.convertToY(data->getOpen(i2));
	break;
      }

      if (! bar.compare(tr("High")))
      {
        y = scaler.convertToY(data->getHigh(i));
        y2 = scaler.convertToY(data->getHigh(i2));
	break;
      }

      if (! bar.compare(tr("Low")))
      {
        y = scaler.convertToY(data->getLow(i));
        y2 = scaler.convertToY(data->getLow(i2));
	break;
      }

      if (! bar.compare(tr("Close")))
      {
        y = scaler.convertToY(data->getClose(i));
        y2 = scaler.convertToY(data->getClose(i2));
	break;
      }

      return;
    }
  }
  else
  {
    y = scaler.convertToY(getValue());
    y2 = scaler.convertToY(getValue2());
  }

  painter.setPen(getColor());
  painter.drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (getExtend())
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    while (x2 < buffer.width())
    {
      x = x2;
      y = y2;
      x2 = x2 + xdiff;
      y2 = y2 - ydiff;
      painter.drawLine (x, y, x2, y2);
    }
  }

  // store the selectable area the line occupies
  clearSelectionArea();
  Q3PointArray array;
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));

  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(tx,
           	  ty - (HANDLE_WIDTH / 2),
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(tx, ty - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(tx2,
             	  ty2 - (HANDLE_WIDTH / 2),
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(tx2, ty2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());
  }

  painter.end();
}

void TrendLine::prefDialog ()
{
  QStringList l;
  l.append(tr("Open"));
  l.append(tr("High"));
  l.append(tr("Low"));
  l.append(tr("Close"));

  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString ub = tr("Use Bar");
  QString el = tr("Extend Line");
  QString bf = tr("Bar Field");
  QString sl = tr("Start Value");
  QString dl = tr("End Value");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit TrendLine"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorPrefItem(cl, pl, color);
  dialog->addComboItem(bf, pl, l, bar);
  dialog->addCheckItem(ub, pl, usebar);
  dialog->addCheckItem(el, pl, extend);
  dialog->addDoubleItem(sl, pl, getValue());
  dialog->addDoubleItem(dl, pl, getValue2());
  dialog->addCheckItem(sd, pl, FALSE);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    dialog->getCombo(bf, bar);
    usebar = dialog->getCheck(ub);
    extend = dialog->getCheck(el);
    setValue(dialog->getDouble(sl));
    setValue2(dialog->getDouble(dl));

    setSaveFlag(TRUE);

    bool f = dialog->getCheck(sd);
    if (f)
    {
      dialog->getColor(cl, defaultColor);
      dialog->getCombo(bf, bar);
      usebar = dialog->getCheck(ub);
      extend = dialog->getCheck(el);

      saveDefaults();
    }

    emit signalDraw();
  }

  delete dialog;
}

void TrendLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  plot = ind;
  name = n;
  mpx2 = -1;
  mpy2 = -1;
  status = ClickWait;
  emit message(tr("Select TrendLine starting point..."));
}

COBase::Status TrendLine::pointerClick (QPoint &point, QDateTime &x, double y)
{
  switch (status)
  {
    case None:
      if (isSelected(point))
      {
        status = Selected;
        emit signalDraw();
      }
      break;
    case Selected:
      moveFlag = isGrabSelected(point);
      if (moveFlag)
        status = Moving;
      else
      {
        if (! isSelected(point))
        {
          status = None;
          emit signalDraw();
        }
      }
      break;
    case Moving:
      status = Selected;
      break;
    case ClickWait:
      tx = x;
      ty = y;
      mpx = point.x();
      mpy = point.y();
      status = ClickWait2;
      emit message(tr("Select TrendLine ending point..."));
      break;
    case ClickWait2:
      if (x <= tx)
        break;

      setDate(tx);
      setDate2(x);
      setValue(ty);
      setValue2(y);

      setSaveFlag(TRUE);
      setColor(defaultColor);
      emit signalDraw();
      status = None;
      emit message("");
      emit signalSave(name);
      break;
    default:
      break;
  }

  return status;
}

void TrendLine::pointerMoving (QPixmap &buffer, QPoint &point, QDateTime &x, double y)
{
  if (status == ClickWait2)
  {
    drawMovingPointer(buffer, point);
    return;
  }

  if (! moveFlag || status != Moving)
    return;

  if (moveFlag == 1)
  {
    if (x >= date2)
      return;

    setDate(x);
    setValue(y);
    setSaveFlag(TRUE);
    emit signalDraw();
    QString s = x.toString("yyyy-MM-dd hh:mm:ss") + " " + QString::number(y);
    emit message(s);
  }
  else
  {
    if (x <= date)
      return;

    setDate2(x);
    setValue2(y);
    setSaveFlag(TRUE);
    emit signalDraw();

    QString s = x.toString("yyyy-MM-dd hh:mm:ss") + " " + QString::number(y);
    emit message(s);
  }
}

void TrendLine::drawMovingPointer (QPixmap &buffer, QPoint &point)
{
  if (point.x() < mpx)
    return;

  QPainter painter;
  painter.begin(&buffer);
  // FIXME: qt3to4: not sure what to do with this
  // painter.setRasterOp(Qt::XorROP);
  painter.setPen(defaultColor);

  // erase the previous line drawn
  if (mpx2 != -1 && mpy2 != -1)
    painter.drawLine (mpx, mpy, mpx2, mpy2);

  // draw the new line
  painter.drawLine (mpx, mpy, point.x(), point.y());

  mpx2 = point.x();
  mpy2 = point.y();

  painter.end();

  emit signalRefresh();
}

void TrendLine::loadDefaults ()
{
  Config settings;
  QString s2 = "DefaultTrendLineColor";
  QString s;
  settings.getData(s2, s);
  if (s.length())
    defaultColor.setNamedColor(s);

  s2 = "DefaultTrendLineBar";
  settings.getData(s2, s);
  if (s.length())
    bar = s;

  s2 = "DefaultTrendLineExtend";
  settings.getData(s2, s);
  if (s.length())
    extend = s.toInt();

  s2 = "DefaultTrendLineUseBar";
  settings.getData(s2, s);
  if (s.length())
    usebar = s.toInt();
}

void TrendLine::saveDefaults ()
{
  Config config;

  QString s = "DefaultTrendLineColor";
  QString s2 = defaultColor.name();
  config.setData(s, s2);

  s = "DefaultTrendLineBar";
  config.setData(s, bar);

  s = "DefaultTrendLineExtend";
  s2 = QString::number(extend);
  config.setData(s, s2);

  s = "DefaultTrendLineUseBar";
  s2 = QString::number(usebar);
  config.setData(s, s2);
}

double TrendLine::getHigh ()
{
  double high = -99999999.0;

  if (getValue() > high)
      high = getValue();
  if (getValue2() > high)
      high = getValue2();

  return high;
}

double TrendLine::getLow ()
{
  double low = 99999999.0;

  if (getValue() < low)
    low = getValue();
  if (getValue2() < low)
    low = getValue2();

  return low;
}

void TrendLine::setDate2 (QDateTime &d)
{
  date2 = d;
}

void TrendLine::getDate2 (QDateTime &d)
{
  d = date2;
}

void TrendLine::setValue2 (double d)
{
  value2 = d;
}

double TrendLine::getValue2 ()
{
  return value2;
}

int TrendLine::isGrabSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return loop + 1;
  }

  return 0;
}

void TrendLine::getSettings (Setting &set)
{
  QString s = date.toString(dateFormat);
  set.setData(sdateLabel, s);
  s = date2.toString(dateFormat);
  set.setData(edateLabel, s);
  s = QString::number(value);
  set.setData(svalueLabel, s);
  s = QString::number(value2);
  set.setData(evalueLabel, s);
  set.setData(fieldLabel, bar);
  s = QString::number(usebar);
  set.setData(usebarLabel, s);
  s = QString::number(extend);
  set.setData(extendLabel, s);
  s = color.name();
  set.setData(colorLabel, s);
  set.setData(plotLabel, plot);
  set.setData(nameLabel, name);
  set.setData(typeLabel, type);
}

void TrendLine::setSettings (Setting &set)
{
  QString s;
  set.getData(colorLabel, s);
  color.setNamedColor(s);
  set.getData(sdateLabel, s);
  Bar b;
  b.setDate(s);
  b.getDate(date);
  set.getData(edateLabel, s);
  b.setDate(s);
  b.getDate(date2);
  value = set.getDouble(svalueLabel);
  value2 = set.getDouble(evalueLabel);
  set.getData(fieldLabel, bar);
  usebar = set.getInt(usebarLabel);
  extend = set.getInt(extendLabel);
  set.getData(plotLabel, plot);
  set.getData(nameLabel, name);
}

bool TrendLine::getUseBar ()
{
  return usebar;
}

void TrendLine::getBar (QString &d)
{
  d = bar;
}

bool TrendLine::getExtend ()
{
  return extend;
}

void TrendLine::adjustForSplit (QDateTime &dt, double d)
{
  if (date < dt)
    value = value * d;
  if (date2 < dt)
    value2 = value2 * d;
}
