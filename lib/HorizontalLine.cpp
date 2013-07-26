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

#include "HorizontalLine.h"
#include "PrefDialog.h"
#include "RcFile.h"
#include <qpainter.h>
#include <q3pointarray.h>
#include <qsettings.h>
//Added by qt3to4:
#include <QPixmap>

HorizontalLine::HorizontalLine ()
{
  defaultColor.setNamedColor("red");
  helpFile = "horizontalline.html";
  text="";
  textLabel = "Text";
  type = "HorizontalLine";

  RcFile rcfile;
  rcfile.loadFont(RcFile::PlotFont,font);

  loadDefaults();
}

HorizontalLine::~HorizontalLine ()
{
}

void HorizontalLine::draw (QPixmap &buffer, Scaler &scaler, int, int, int)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(font);

  int y = scaler.convertToY(getValue());

  // if value is off chart then don't draw it
  if (getValue() < scaler.getLow())
    return;

  painter.setPen(getColor());

  QFontMetrics fm(font);
  QString s;
  getText(s);
  int pixelsWide = fm.width(s);
  painter.drawLine (0, y, buffer.width(), y);
  painter.drawText(0, y - 1, s, -1);
  painter.drawText(0 + pixelsWide + 1, y - 1, QString::number(getValue()), -1);

  clearSelectionArea();
  Q3PointArray array;
  array.putPoints(0, 4, 0, y - 4, 0, y + 4, buffer.width(), y + 4, buffer.width(), y - 4);
  setSelectionArea(new QRegion(array));

  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    int t = (int) buffer.width() / 4;

    setGrabHandle(new QRegion(0,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(0, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(t,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
			      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(t * 2,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
		              HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t * 2, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(t * 3,
             		      y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
		  	      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t * 3, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());

    setGrabHandle(new QRegion(t * 4,
             	              y - (HANDLE_WIDTH / 2),
			      HANDLE_WIDTH,
		    	      HANDLE_WIDTH,
			      QRegion::Rectangle));
    painter.fillRect(t * 4, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, getColor());
  }

  painter.end();
}

void HorizontalLine::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString vl = tr("Value");
  QString tx = tr("Text");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit HorizontalLine"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorPrefItem(cl, pl, color);
  dialog->addDoubleItem(vl, pl, getValue());
  dialog->addTextItem(tx, pl, text);  //cz odkazy na objekty definovane nahore
  dialog->addCheckItem(sd, pl, FALSE);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    value = dialog->getDouble(vl);
    dialog->getText(tx, text);
    setSaveFlag(TRUE);

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

void HorizontalLine::newObject (QString &ind, QString &n)
{
  indicator = ind;
  plot = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place HorizontalLine..."));
}

COBase::Status HorizontalLine::pointerClick (QPoint &point, QDateTime &, double y)
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
      setValue(y);
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

void HorizontalLine::pointerMoving (QPixmap &, QPoint &, QDateTime &, double y)
{
  if (status != Moving)
    return;

  setValue(y);
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = QString::number(y);
  emit message(s);
}

void HorizontalLine::loadDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultHorizontalLineColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void HorizontalLine::saveDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultHorizontalLineColor";
  settings.writeEntry(s, defaultColor.name());
}

void HorizontalLine::getSettings (Setting &set)
{
  QString s = QString::number(value);
  set.setData(valueLabel, s);
  s = color.name();
  set.setData(colorLabel, s);
  set.setData(plotLabel, plot);
  set.setData(nameLabel, name);
  set.setData(textLabel, text);
  set.setData(typeLabel, type);
}

void HorizontalLine::setSettings (Setting &set)
{
  QString s;
  value = set.getDouble(valueLabel);
  set.getData(colorLabel, s);
  color.setNamedColor(s);
  set.getData(plotLabel, plot);
  set.getData(nameLabel, name);
  set.getData(textLabel, text);
}

void HorizontalLine::getText (QString &d)
{
  d = text;
}

void HorizontalLine::adjustForSplit (QDateTime &, double d)
{
  value = value * d;
}
