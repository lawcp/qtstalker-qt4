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

#include "COBase.h"
#include "../pics/deleteitem.xpm"
#include "../pics/edit.xpm"
//#include "../pics/renameitem.xpm"
#include "BuyArrow.h"
#include "Cycle.h"
#include "FiboLine.h"
#include "HorizontalLine.h"
#include "SellArrow.h"
#include "Text.h"
#include "TrendLine.h"
#include "VerticalLine.h"
#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>
#include <QKeyEvent>
#include <QMenu>

COBase::COBase ()
{
  data = 0;
  menu = new QMenu();
  status = None;
  saveFlag = FALSE;
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  date = QDateTime::currentDateTime();

  dateFormat = "yyyyMMddhhmmss";
  dateLabel = "Date";
  valueLabel = "Value";
  colorLabel = "Color";
  plotLabel = "Plot";
  nameLabel = "Name";
  typeLabel = "Type";

  menu->insertItem(QPixmap(edit), tr("&Edit Object"), this, SLOT(prefDialog()), Qt::CTRL+Qt::Key_E);
//  menu->insertItem(QPixmap(renameitem), tr("&Move Object"), this, SLOT(moveObject()), Qt::CTRL+Qt::Key_M);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Object"), this, SLOT(removeObject()), Qt::CTRL+Qt::Key_D);
}

COBase::~COBase ()
{
  delete menu;
}

void COBase::draw (QPixmap &, Scaler &, int, int, int)
{
}

void COBase::prefDialog ()
{
}

void COBase::showMenu ()
{
  menu->exec(QCursor::pos());
}

void COBase::setData (BarData *d)
{
  data = d;
}

void COBase::newObject (QString &, QString &)
{
}

COBase::Status COBase::pointerClick (QPoint &, QDateTime &, double)
{
  return COBase::None;
}

void COBase::pointerMoving (QPixmap &, QPoint &, QDateTime &, double)
{
}

void COBase::keyEvent (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_M:
      if (key->state() == Qt::ControlButton)
        moveObject();
      break;
    case Qt::Key_E:
      if (key->state() == Qt::ControlButton)
        prefDialog();
      break;
    case Qt::Key_D:
      if (key->state() == Qt::ControlButton)
        removeObject();
      break;
    default:
      key->ignore();
      break;
  }
}

void COBase::getSettings (Setting &)
{
}

void COBase::setSettings (Setting &)
{
}

double COBase::getHigh ()
{
  return value;
}

double COBase::getLow ()
{
  return value;
}

QString COBase::getHelpFile ()
{
  return helpFile;
}

void COBase::setSaveFlag (bool d)
{
  saveFlag = d;
}

bool COBase::getSaveFlag ()
{
  return saveFlag;
}

void COBase::clearGrabHandles ()
{
  grabHandles.clear();
}

void COBase::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void COBase::clearSelectionArea ()
{
  selectionArea.clear();
}

void COBase::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

QString COBase::getName ()
{
  return name;
}

void COBase::setDate (QDateTime &d)
{
  date = d;
}

void COBase::getDate (QDateTime &d)
{
  d = date;
}

void COBase::setColor (QColor d)
{
  color =d;
}

QColor COBase::getColor ()
{
  return color;
}

void COBase::setValue (double d)
{
  value = d;
}

double COBase::getValue ()
{
  return value;
}

void COBase::setStatus (Status d)
{
  status = d;
}

COBase::Status COBase::getStatus ()
{
  return status;
}

bool COBase::isSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) selectionArea.count(); loop++)
  {
    QRegion *r = selectionArea.at(loop);
    if (r->contains(point))
      return TRUE;
  }

  return FALSE;
}

bool COBase::isGrabSelected (QPoint point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return TRUE;
  }

  return FALSE;
}

void COBase::removeObject ()
{
  emit signalObjectDeleted(name);
}

void COBase::moveObject ()
{
  status = Moving;
}

void COBase::addObject (Setting &set)
{
  setSettings(set);
}

void COBase::adjustForSplit (QDateTime &, double)
{
}

COBase * COBase::getCO (Setting &set)
{
  QString s;
  set.getData(typeLabel, s);
  return getCO(s);
}

COBase * COBase::getCO (QString &s)
{
  COBase *t = 0;

  if (! s.compare("BuyArrow"))
    t = new BuyArrow();
  else
  {
    if (! s.compare("Cycle"))
      t = new Cycle();
    else
    {
      if (! s.compare("FiboLine"))
        t = new FiboLine();
      else
      {
        if (! s.compare("HorizontalLine"))
          t = new HorizontalLine();
        else
        {
          if (! s.compare("SellArrow"))
            t = new SellArrow();
          else
          {
            if (! s.compare("Text"))
              t = new Text();
            else
            {
              if (! s.compare("TrendLine"))
                t = new TrendLine();
              else
              {
                if (! s.compare("VerticalLine"))
                  t = new VerticalLine();
              }
            }
          }
        }
      }
    }
  }

  return t;
}
