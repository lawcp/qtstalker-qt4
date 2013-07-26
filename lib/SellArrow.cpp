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

#include "SellArrow.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>
//Added by qt3to4:
#include <QPixmap>

SellArrow::SellArrow ()
{
  defaultColor.setNamedColor("red");
  helpFile = "sellarrow.html";
  identifierLabel = "Identifier";
  priceLabel = "Price";
  type = "SellArrow";

  loadDefaults();
}

SellArrow::~SellArrow ()
{
}

void SellArrow::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  int y = scaler.convertToY(getValue());

  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
	          x + 2, y - 5,
	          x + 2, y - 11,
	          x - 2, y - 11,
	          x - 2, y - 5,
                  x - 5, y - 5);
  painter.setBrush(getColor());
  painter.drawPolygon(arrow, TRUE, 0, -1);

  clearSelectionArea();
  setSelectionArea(new QRegion(arrow));

  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - (HANDLE_WIDTH / 2),
             	  y + 1,
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));

    painter.fillRect(x - (HANDLE_WIDTH / 2), y + 1, HANDLE_WIDTH, HANDLE_WIDTH, getColor());
  }

  painter.end();
}

void SellArrow::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString vl = tr("Value");
  QString il = tr("Identifier");
  QString bl = tr("Price");
  QString sd = tr("Set Default");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit SellArrow"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorPrefItem(cl, pl, color);
  dialog->addDoubleItem(vl, pl, getValue());
  dialog->addTextItem(il, pl, identifier);
  dialog->addTextItem(bl, pl, price);
  dialog->addCheckItem(sd, pl, FALSE);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    value = dialog->getDouble(vl);
    dialog->getText(il, identifier);
    dialog->getText(bl, price);

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

void SellArrow::newObject (QString &ind, QString &n)
{
  indicator = ind;
  plot = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place SellArrow..."));
}

COBase::Status SellArrow::pointerClick (QPoint &point, QDateTime &x, double y)
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

void SellArrow::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
{
  if (status != Moving)
    return;

  setDate(x);
  setValue(y);
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = x.toString("yyyy-MM-dd hh:mm:ss") + " " + QString::number(y);
  emit message(s);
}

void SellArrow::loadDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultSellArrowColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);
}

void SellArrow::saveDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultSellArrowColor";
  settings.writeEntry(s, defaultColor.name());
}

void SellArrow::getSettings (Setting &set)
{
  QString s = date.toString(dateFormat);
  set.setData(dateLabel, s);
  s = QString::number(value);
  set.setData(valueLabel, s);
  s = color.name();
  set.setData(colorLabel, s);
  set.setData(identifierLabel, identifier);
  set.setData(priceLabel, price);
  set.setData(plotLabel, plot);
  set.setData(nameLabel, name);
  set.setData(typeLabel, type);
}

void SellArrow::setSettings (Setting &set)
{
  QString s;
  set.getData(dateLabel, s);
  Bar bar;
  bar.setDate(s);
  bar.getDate(date);
  value = set.getDouble(valueLabel);
  set.getData(colorLabel, s);
  color.setNamedColor(s);
  set.getData(plotLabel, plot);
  set.getData(identifierLabel, identifier);
  set.getData(priceLabel, price);
  set.getData(nameLabel, name);
}

void SellArrow::adjustForSplit (QDateTime &dt, double d)
{
  if (date < dt)
    value = value * d;
}
