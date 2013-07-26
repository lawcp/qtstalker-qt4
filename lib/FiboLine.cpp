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

#include "FiboLine.h"
#include "PrefDialog.h"
#include "RcFile.h"
#include <qpainter.h>
#include <qsettings.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QPixmap>

FiboLine::FiboLine ()
{
  defaultColor.setNamedColor("red");
  helpFile = "fiboline.html";
  extend = FALSE;
  line1 = 0.382;
  line2 = 0.5;
  line3 = 0.618;
  line4 = 0;
  line5 = 0;
  line6 = 0;
  startDate = QDateTime::currentDateTime();
  endDate = startDate;
  type = "FiboLine";

  highLabel = "High";
  lowLabel = "Low";
  sdateLabel = "Start Date";
  edateLabel = "End Date";
  l1Label = "Line 1";
  l2Label = "Line 2";
  l3Label = "Line 3";
  l4Label = "Line 4";
  l5Label = "Line 5";
  l6Label = "Line 6";
  extendLabel = "Extend";

  RcFile rcfile;
  rcfile.loadFont(RcFile::PlotFont,font);

  loadDefaults();
}

FiboLine::~FiboLine ()
{
}

void FiboLine::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(font);

  int x2 = data->getX(startDate);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  QDateTime dt = endDate;
  if (extend)
    data->getDate(data->count() - 1, dt);

  x2 = data->getX(dt);
  if (x2 == -1)
    return;

  x2 = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x2 == -1)
    return;

  painter.setPen(getColor());

  Q3PointArray array;
  clearSelectionArea();

  int loop;
  for (loop = 1; loop <= 6; loop++)
  {
    if (getLine(loop) != 0)
    {
      double r = getY(getLine(loop), getHigh(), getLow());
      int y = scaler.convertToY(r);
      painter.drawLine (x, y, x2, y);
      painter.drawText(x, y - 1, QString::number(getLine(loop) * 100) + "% - " + QString::number(r), -1);

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      setSelectionArea(new QRegion(array));
    }
  }

  // draw the low line
  int y = scaler.convertToY(getLow());
  painter.drawLine (x, y, x2, y);
  // if (co->getStatus() == FiboLineObject::Selected)
  painter.drawText(x, y - 1, "0% - " + QString::number(getLow()), -1);

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  setSelectionArea(new QRegion(array));

  // draw the high line
  int y2 = scaler.convertToY(getHigh());
  painter.drawLine (x, y2, x2, y2);
  // if (co->getStatus() == FiboLineObject::Selected)
  painter.drawText(x, y2 - 1, "100% - " + QString::number(getHigh()), -1);

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  setSelectionArea(new QRegion(array));

  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();

    //bottom left corner
    y = scaler.convertToY(getLow());
    setGrabHandle(new QRegion(x,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(x, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    //top right corner
    y2 = scaler.convertToY(getHigh());
    setGrabHandle(new QRegion(x2,
             		      y2 - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(x2, y2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());
  }

  painter.end();
}

double FiboLine::getY (double v, double high, double low)
{
  double range = high - low;
  double r = 0;
  if (v < 0)
    r = low + (range * v);
  else
  {
    if (v > 0)
      r = low + (range * v);
    else
    {
      if (v < 0)
        r = high;
      else
        r = low;
    }
  }

  return r;
}

void FiboLine::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString l1 = tr("Line 1");
  QString l2 = tr("Line 2");
  QString l3 = tr("Line 3");
  QString l4 = tr("Line 4");
  QString l5 = tr("Line 5");
  QString l6 = tr("Line 6");
  QString hl = tr("High");
  QString ll = tr("Low");
  QString el = tr("Extend");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit FiboLine"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorPrefItem(cl, pl, color);
  dialog->addDoubleItem(hl, pl, getHigh());
  dialog->addDoubleItem(ll, pl, getLow());
  dialog->addCheckItem(el, pl, extend);
  dialog->addCheckItem(sd, pl, FALSE);

  pl = tr("Levels");
  dialog->createPage (pl);
  dialog->addDoubleItem(l1, pl, getLine(1));
  dialog->addDoubleItem(l2, pl, getLine(2));
  dialog->addDoubleItem(l3, pl, getLine(3));
  dialog->addDoubleItem(l4, pl, getLine(4));
  dialog->addDoubleItem(l5, pl, getLine(5));
  dialog->addDoubleItem(l6, pl, getLine(6));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    line1 = dialog->getDouble(l1);
    line2 = dialog->getDouble(l2);
    line3 = dialog->getDouble(l3);
    line4 = dialog->getDouble(l4);
    line5 = dialog->getDouble(l5);
    line6 = dialog->getDouble(l6);
    high = dialog->getDouble(hl);
    low = dialog->getDouble(ll);
    extend = dialog->getCheck(el);

    setSaveFlag(TRUE);

    bool f = dialog->getCheck(sd);
    if (f)
    {
      dialog->getColor(cl, defaultColor);
      line1 = dialog->getDouble(l1);
      line2 = dialog->getDouble(l2);
      line3 = dialog->getDouble(l3);
      line4 = dialog->getDouble(l4);
      line5 = dialog->getDouble(l5);
      line6 = dialog->getDouble(l6);

      saveDefaults();
    }

    emit signalDraw();
  }

  delete dialog;
}

void FiboLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  plot = ind;
  name = n;
  mpx2 = -1;
  mpy2 = -1;
  status = ClickWait;
  emit message(tr("Select FiboLine high point..."));
}

COBase::Status FiboLine::pointerClick (QPoint &point, QDateTime &x, double y)
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
      emit message(tr("Select FiboLine low point..."));
      break;
    case ClickWait2:
      if (x <= tx)
        break;
      if (y >= ty)
        break;
      status = None;
      emit message("");
      break;
    default:
      break;
  }

  return status;
}

void FiboLine::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
{
  if (status == ClickWait2)
  {
    startDate = tx;
    endDate = x;
    high = ty;
    low = y;
    setSaveFlag(TRUE);
    setColor(defaultColor);
    emit signalDraw();
    QString s = x.toString("yyyy-MM-dd hh:mm:ss") + " " + QString::number(y);
    emit message(s);
    emit signalSave(name);
    return;
  }

  if (! moveFlag || status != Moving)
    return;

  if (moveFlag == 1)
  {
    // bottom left corner
    if (x >= endDate)
      return;

    if (y >= getHigh())
      return;

    startDate = x;
    low = y;
    setSaveFlag(TRUE);

    emit signalDraw();

    QString s = x.toString("yyyy-MM-dd hh:mm:ss") + " " + QString::number(y);
    emit message(s);
  }
  else
  {
    //top right corner
    if (x <= startDate)
      return;

    if (y <= getLow())
      return;

    endDate = x;
    high = y;
    setSaveFlag(TRUE);

    emit signalDraw();

    QString s = x.toString("yyyy-MM-dd hh:mm:ss") + " " + QString::number(y);
    emit message(s);
  }
}

void FiboLine::loadDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultFiboLineColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);

  s = "/Qtstalker/DefaultFiboLine1";
  s = settings.readEntry(s);
  if (s.length())
    line1 = s.toFloat();

  s = "/Qtstalker/DefaultFiboLine2";
  s = settings.readEntry(s);
  if (s.length())
    line2 = s.toFloat();

  s = "/Qtstalker/DefaultFiboLine3";
  s = settings.readEntry(s);
  if (s.length())
    line3 = s.toFloat();

  s = "/Qtstalker/DefaultFiboLine4";
  s = settings.readEntry(s);
  if (s.length())
    line4 = s.toFloat();

  s = "/Qtstalker/DefaultFiboLine5";
  s = settings.readEntry(s);
  if (s.length())
    line5 = s.toFloat();

  s = "/Qtstalker/DefaultFiboLine6";
  s = settings.readEntry(s);
  if (s.length())
    line6 = s.toFloat();
}

void FiboLine::saveDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultFiboLineColor";
  settings.writeEntry(s, defaultColor.name());

  s = "/Qtstalker/DefaultFiboLine1";
  settings.writeEntry(s, QString::number(line1));

  s = "/Qtstalker/DefaultFiboLine2";
  settings.writeEntry(s, QString::number(line2));

  s = "/Qtstalker/DefaultFiboLine3";
  settings.writeEntry(s, QString::number(line3));

  s = "/Qtstalker/DefaultFiboLine4";
  settings.writeEntry(s, QString::number(line4));

  s = "/Qtstalker/DefaultFiboLine5";
  settings.writeEntry(s, QString::number(line5));

  s = "/Qtstalker/DefaultFiboLine6";
  settings.writeEntry(s, QString::number(line6));
}

double FiboLine::getHigh ()
{
  return high;
}

double FiboLine::getLow ()
{
  return low;
}

void FiboLine::getSettings (Setting &set)
{
  QString s = color.name();
  set.setData(colorLabel, s);
  set.setData(plotLabel, plot);
  set.setData(nameLabel, name);
  s = QString::number(high);
  set.setData(highLabel, s);
  s = QString::number(low);
  set.setData(lowLabel, s);
  s = startDate.toString(dateFormat);
  set.setData(sdateLabel, s);
  s = endDate.toString(dateFormat);
  set.setData(edateLabel, s);
  s = QString::number(line1);
  set.setData(l1Label, s);
  s = QString::number(line2);
  set.setData(l2Label, s);
  s = QString::number(line3);
  set.setData(l3Label, s);
  s = QString::number(line4);
  set.setData(l4Label, s);
  s = QString::number(line5);
  set.setData(l5Label, s);
  s = QString::number(line6);
  set.setData(l6Label, s);
  s = QString::number(extend);
  set.setData(extendLabel, s);
  set.setData(typeLabel, type);
}

void FiboLine::setSettings (Setting &set)
{
  QString s;
  set.getData(colorLabel, s);
  color.setNamedColor(s);
  set.getData(plotLabel, plot);
  set.getData(nameLabel, name);
  high = set.getDouble(highLabel);
  low = set.getDouble(lowLabel);
  set.getData(sdateLabel, s);
  Bar bar;
  bar.setDate(s);
  bar.getDate(startDate);
  set.getData(edateLabel, s);
  bar.setDate(s);
  bar.getDate(endDate);
  line1 = set.getDouble(l1Label);
  line2 = set.getDouble(l2Label);
  line3 = set.getDouble(l3Label);
  line4 = set.getDouble(l4Label);
  line5 = set.getDouble(l5Label);
  line6 = set.getDouble(l6Label);
  extend = set.getInt(extendLabel);
}

int FiboLine::isGrabSelected (QPoint point)
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

double FiboLine::getLine (int i)
{
  double v = 0.0;

  switch (i)
  {
    case 1:
      v = line1;
      break;
    case 2:
      v = line2;
      break;
    case 3:
      v = line3;
      break;
    case 4:
      v = line4;
      break;
    case 5:
      v = line5;
      break;
    case 6:
      v = line6;
      break;
    default:
      break;
  }

  return v;
}

void FiboLine::adjustForSplit (QDateTime &dt, double d)
{
  if (startDate < dt && endDate < dt)
  {
    high = high * d;
    low = low * d;
  }
}
