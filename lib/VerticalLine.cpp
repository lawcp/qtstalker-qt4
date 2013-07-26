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

#include "VerticalLine.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3PointArray>

VerticalLine::VerticalLine ()
{
  defaultColor.setNamedColor("red");
  helpFile = "verticalline.html";
  type = "VerticalLine";

  loadDefaults();
}

VerticalLine::~VerticalLine ()
{
}

void VerticalLine::draw (QPixmap &buffer, Scaler &, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  painter.setPen(getColor());

  painter.drawLine (x, 0, x, buffer.height());

  clearSelectionArea();
  Q3PointArray array;
  array.putPoints(0,
  	          4,
		  x - (HANDLE_WIDTH / 2), 0,
		  x + (HANDLE_WIDTH / 2), 0,
		  x + (HANDLE_WIDTH / 2), buffer.height(),
		  x - (HANDLE_WIDTH / 2), buffer.height());
  setSelectionArea(new QRegion(array));

  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();

    int t = (int) buffer.height() / 4;

    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  0,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), 0, HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
            	  t,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t, HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
            	  t * 2,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t * 2, HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  t * 3,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t * 3, HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  t * 4,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
    painter.fillRect(x - (HANDLE_WIDTH / 2), t * 4, HANDLE_WIDTH, HANDLE_WIDTH, getColor());
  }

  painter.end();
}

void VerticalLine::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit VerticalLine"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorPrefItem(cl, pl, color);
  dialog->addCheckItem(sd, pl, FALSE);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);

    bool f = dialog->getCheck(sd);
    if (f)
    {
      defaultColor = color;
      saveDefaults();
    }

    emit signalDraw();
  }

  delete dialog;
}

void VerticalLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  plot = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place VerticalLine..."));
}

COBase::Status VerticalLine::pointerClick (QPoint &point, QDateTime &x, double)
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
      if (isGrabSelected(point))
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
      setDate(x);
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

void VerticalLine::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double)
{
  if (status != Moving)
    return;

  setDate(x);
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = x.toString("yyyy-MM-dd hh:mm:ss");
  emit message(s);
}

void VerticalLine::loadDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultVerticalLineColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void VerticalLine::saveDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultVerticalLineColor";
  settings.writeEntry(s, defaultColor.name());
}

double VerticalLine::getHigh ()
{
  return data->getMax();
}

double VerticalLine::getLow ()
{
  return data->getMin();
}

void VerticalLine::getSettings (Setting &set)
{
  QString s = date.toString(dateFormat);
  set.setData(dateLabel, s);
  s = color.name();
  set.setData(colorLabel, s);
  set.setData(plotLabel, plot);
  set.setData(nameLabel, name);
  set.setData(typeLabel, type);
}

void VerticalLine::setSettings (Setting &set)
{
  QString s;
  set.getData(dateLabel, s);
  Bar bar;
  bar.setDate(s);
  bar.getDate(date);
  set.getData(colorLabel, s);
  color.setNamedColor(s);
  set.getData(plotLabel, plot);
  set.getData(nameLabel, name);
}
