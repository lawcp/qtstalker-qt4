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

#include "Text.h"
#include "PrefDialog.h"
#include "Config.h"
#include <qpainter.h>
#include <qsettings.h>
//Added by qt3to4:
#include <QPixmap>

Text::Text ()
{
  defaultColor.setNamedColor("red");
  helpFile = "text.html";
  label = "Text";
  fontLabel = "Font";
  labelLabel = "Label";
  type = "Text";

  RcFile rcfile;
  rcfile.loadFont(RcFile::PlotFont,font);

  loadDefaults();
}

Text::~Text ()
{
}

void Text::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);

  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  painter.setFont(font);
  painter.setPen(color);

  int y = scaler.convertToY(getValue());

  painter.drawText(x, y, label);

  QFontMetrics fm = painter.fontMetrics();
  clearSelectionArea();
  setSelectionArea(new QRegion(x,
                   y - fm.height(),
		   fm.width(label, -1),
		   fm.height(),
		   QRegion::Rectangle));

  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - HANDLE_WIDTH - 1,
             	  y - (fm.height() / 2),
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));

    painter.fillRect(x - HANDLE_WIDTH - 1,
                     y - (fm.height() / 2),
		     HANDLE_WIDTH,
		     HANDLE_WIDTH,
		     getColor());
  }

  painter.end();
}

void Text::prefDialog ()
{
  QString pl = tr("Details");
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString fl = tr("Font");
  QString ll = tr("Label");
  QString vl = tr("Value");

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Text"));
  dialog->createPage (pl);
  dialog->setHelpFile (helpFile);
  dialog->addColorPrefItem(cl, pl, color);
  dialog->addFontItem(fl, pl, font);
  dialog->addTextItem(ll, pl, label);
  dialog->addDoubleItem(vl, pl, getValue());
  dialog->addCheckItem(sd, pl, FALSE);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    dialog->getText(ll, label);
    dialog->getFont(fl, font);
    value = dialog->getDouble(vl);

    setSaveFlag(TRUE);

    bool f = dialog->getCheck(sd);
    if (f)
    {
      dialog->getColor(cl, defaultColor);
      dialog->getFont(fl, dfont);
      saveDefaults();
    }

    emit signalDraw();
  }

  delete dialog;
}

void Text::newObject (QString &ind, QString &n)
{
  indicator = ind;
  plot = ind;
  name = n;
  status = ClickWait;
  emit message(tr("Select point to place Text..."));
}

COBase::Status Text::pointerClick (QPoint &point, QDateTime &x, double y)
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

void Text::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
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

void Text::loadDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultTextColor";
  s = settings.readEntry(s);
  if (s.length())
    defaultColor.setNamedColor(s);

  s = "/Qtstalker/DefaultTextFont";
  s = settings.readEntry(s);
  if (s.length())
  {
    QStringList l = QStringList::split(",", s, FALSE);
    font = QFont(l[0], l[1].toInt(), l[2].toInt());
  }
}

void Text::saveDefaults ()
{
  QSettings settings;

  QString s = "/Qtstalker/DefaultTextColor";
  settings.writeEntry(s, defaultColor.name());

  s = "/Qtstalker/DefaultTextFont";
  settings.writeEntry(s, dfont.family() + "," +
                      QString::number(dfont.pointSize()) + "," +
		      QString::number(dfont.weight()));
}

void Text::getSettings (Setting &set)
{
  QString s = date.toString(dateFormat);
  set.setData(dateLabel, s);
  s = QString::number(value);
  set.setData(valueLabel, s);
  s = color.name();
  set.setData(colorLabel, s);
  set.setData(plotLabel, plot);
  set.setData(nameLabel, name);
  set.setData(labelLabel, label);
  set.setData(typeLabel, type);

  s = font.family();
  s.append(" ");
  s.append(QString::number(font.pointSize()));
  s.append(" ");
  s.append(QString::number(font.weight()));
  set.setData(fontLabel, s);
}

void Text::setSettings (Setting &set)
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
  set.getData(nameLabel, name);
  set.getData(labelLabel, label);

  set.getData(fontLabel, s);
  QStringList l = QStringList::split(" ", s, FALSE);
  if (l.count())
    font = QFont(l[0], l[1].toInt(), l[2].toInt());
}

void Text::getFont (QFont &d)
{
  d = font;
}

void Text::getLabel (QString &d)
{
  d = label;
}

void Text::adjustForSplit (QDateTime &dt, double d)
{
  if (date < dt)
    value = value * d;
}
