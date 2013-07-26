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

#include "ScalePlot.h"
#include <qpainter.h>
#include <qpen.h>
#include <qpoint.h>
#include <q3pointarray.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QPaintEvent>
#include <math.h>
#include <q3paintdevicemetrics.h>
#include <qstring.h>
#include <QVector>

#define SCALE_WIDTH 60

ScalePlot::ScalePlot (QWidget *w) : QWidget(w)
{
  setBackgroundMode(Qt::NoBackground);
  scaleWidth = SCALE_WIDTH;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  mainFlag = FALSE;
  scaleToScreen = FALSE;
  logScale = FALSE;
  close = 0;
  activeFlag = FALSE;

  plotFont.setFamily("DejaVu Sans");
  plotFont.setPointSize(10);
  plotFont.setWeight(50);

  setMinimumWidth(scaleWidth);
  setMaximumWidth(scaleWidth);
}

ScalePlot::~ScalePlot ()
{
}

void ScalePlot::clear ()
{
  close = 0;
}

void ScalePlot::setData (double c)
{
  close = c;
  activeFlag = TRUE;
}

void ScalePlot::setMainFlag (bool d)
{
  mainFlag = d;
}

void ScalePlot::setScaleToScreen (bool d)
{
  scaleToScreen = d;
}

void ScalePlot::setLogScale (bool d)
{
  logScale = d;
}

void ScalePlot::draw ()
{
  buffer.fill(backgroundColor);

  if (activeFlag)
  {
    if (buffer.isNull())
    {
      buffer.resize(this->width(), this->height());
      buffer.fill(backgroundColor);
    }

    drawScale();
  }

  update();
}

void ScalePlot::drawRefresh ()
{
  update();
}

void ScalePlot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, &buffer);
}

void ScalePlot::resizeEvent (QResizeEvent *event)
{
  buffer.resize(event->size());
  draw();
}

void ScalePlot::setScaleWidth (int d)
{
  if (d > 999 || d < SCALE_WIDTH)
    return;
  else
    scaleWidth = d;
}

void ScalePlot::setBackgroundColor (QColor &d)
{
  backgroundColor = d;
}

void ScalePlot::setBorderColor (QColor &d)
{
  borderColor = d;
}

void ScalePlot::setPlotFont (QFont &d)
{
  plotFont = d;
}

void ScalePlot::drawScale ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);
  painter.setPen(QPen(borderColor, 1, Qt::SolidLine));

  painter.fillRect(0, 0, buffer.width(), buffer.height(), backgroundColor);

  QVector<double> scaleArray;
  scaler.getScaleArray(scaleArray);

  QFontMetrics fm(plotFont);

  int x = 0;
  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray[loop]);
    painter.drawLine (x, y, x + 4, y);

    // draw the text
    QString s;
    strip(scaleArray[loop], 4, s);

    // abbreviate too many (>=3) trailing zeroes in large numbers on y-axes
    if (! mainFlag)
    {
      bool flag = FALSE;

      if (s.toDouble() < 0)
      {
        flag = TRUE;
	s.remove(0, 1);
      }

      if (s.toDouble() >= 1000000000)
      {
        strip(s.toDouble() / 1000000000, 4, s);
	s.append("b");
      }
      else
      {
        if (s.toDouble() >= 1000000)
        {
          strip(s.toDouble() / 1000000, 4, s);
	  s.append("m");
        }
//        else
//        {
//          if (s.toDouble() >= 1000)
//          {
//            strip(s.toDouble() / 1000, 4, s);
//            s.append("k");
//          }
//        }
      }

      if (flag)
        s.prepend("-");
    }

    painter.drawText(x + 7, y + (fm.height() / 2), s);
  }

  painter.drawLine (x, 0, x, buffer.height());

  // draw the last value pointer on the scale of main plot
  int y = scaler.convertToY(close);

  Q3PointArray array;
  array.setPoints(3, x + 2, y,
                  x + 8, y - 4,
                  x + 8, y + 4);
  painter.setBrush(borderColor);
  painter.drawPolygon(array, TRUE, 0, -1);

  painter.end();
}

void ScalePlot::strip (double d, int p, QString &s)
{
  s = QString::number(d, 'f', p);

  while (1)
  {
    if (s.find('.', -1, TRUE) != -1)
    {
      s.truncate(s.length() - 1);
      break;
    }
    else
    {
      if (s.find('0', -1, TRUE) != -1)
        s.truncate(s.length() - 1);
      else
        break;
    }
  }
}

void ScalePlot::slotScaleToScreenChanged (bool d)
{
  setScaleToScreen(d);
  draw();
}

void ScalePlot::slotLogScaleChanged (bool d)
{
  setLogScale(d);
  draw();
}

void ScalePlot::setScaler (Scaler &d)
{
  scaler = d;
}
