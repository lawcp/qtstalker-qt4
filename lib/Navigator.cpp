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

#include "Navigator.h"
#include "../pics/dirclosed.xpm"
#include <qpixmap.h>
//Added by qt3to4:
#include <QKeyEvent>

Navigator::Navigator (QWidget *w, QString &bp) : Q3ListBox(w)
{
  basePath = bp;
  id = 0;
  keyFlag = FALSE;
  selectedFlag = FALSE;

  currentDir.setPath(bp);
  currentDir.setMatchAllDirs(TRUE);
  setSelectionMode(Q3ListBox::Single);
  /*
  connect(this, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(fileSelection(QListBoxItem *)));
  connect(this, SIGNAL(selected(QListBoxItem *)), this, SLOT(checkDirectory(QListBoxItem *)));
  */
  connect(this, SIGNAL(currentChanged(Q3ListBoxItem *)), this, SLOT(fileSelection(Q3ListBoxItem *)));
  connect(this, SIGNAL(returnPressed (Q3ListBoxItem *)), this, SLOT(checkDirectory(Q3ListBoxItem *)));
  connect(this, SIGNAL(mouseButtonClicked(int, Q3ListBoxItem *, const QPoint &)), this, SLOT(slotMouseClicked(int, Q3ListBoxItem *, const QPoint &)));
  //connect(this, SIGNAL(highlighted(QListBoxItem *)), this, SLOT(highL(QListBoxItem *)));
}

Navigator::~Navigator ()
{
}

void Navigator::updateList ()
{
  int ci = currentItem();

  clear();
  currentDir.setPath(currentDir.absPath());

  int loop;
  if (! basePath.compare(currentDir.absPath()))
  {
    loop = 1;
    currentDir.setFilter(QDir::Dirs);
  }
  else
  {
    loop = 0;
    currentDir.setFilter(QDir::All);
  }

  for (; loop < (int) currentDir.count(); loop++)
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(currentDir[loop]);
    QFileInfo info(s);

    if (info.isDir())
    {
      if (currentDir[loop].compare("."))
        insertItem(QPixmap(dirclosed), currentDir[loop], -1);
    }
    else
    {
      // check if this is a broken group link
      if (! info.exists())
      {
        QDir dir;
	dir.remove(s);
	continue;
      }

      insertItem(currentDir[loop], -1);
    }
  }

  clearSelection();

  if (ci != -1)
    setCurrentItem(ci);
}

void Navigator::setNavItem (QString s)
{
  int loop;
  for (loop = 0; loop < (int) currentDir.count(); loop++)
  {
    if ( !s.compare(currentDir[loop]) )
    {
      setCurrentItem(loop-1); // allow for . and .. dirs
      return;
    }
  }
}

void Navigator::upDirectory ()
{
  QString s = currentDir.dirName();
  if (s == "data" || s == "group")
    return;
  currentDir.cdUp();
  updateList();
  setCurrentItem(findItem(s, Q3ListBox::ExactMatch));
  ensureCurrentVisible();
  emit noSelection();
  selectedFlag = FALSE;
}

void Navigator::fileSelection (Q3ListBoxItem *item)
{
  //qDebug("fileSelection(currentChanged)");
  if (! item)
  {
    emit noSelection();
    selectedFlag = FALSE;
    return;
  }

  if (item->pixmap())
  {
    emit noSelection();
    selectedFlag = FALSE;
    return;
  }

  QString s;
  getFileSelection(s);
  emit fileSelected(s);
  selectedFlag = TRUE;
}

void Navigator::getFileSelection (QString &s)
{
  s = currentDir.absPath();
  s.append("/");
  s.append(currentText());
}

void Navigator::setDirectory (QString &d)
{
  if (d.length())
  {
  //  QFileInfo fi(d);
  //  currentDir.setPath(fi.dirPath(TRUE));
    currentDir.setPath(d);
    updateList();
    emit noSelection();
    selectedFlag = FALSE;
  }
}

void Navigator::getCurrentPath (QString &d)
{
  d = currentDir.absPath();
}

void Navigator::checkDirectory (Q3ListBoxItem *item)
{
  //qDebug("checkDirectory(selected)");
  if (! item)
  {
    emit noSelection();
    selectedFlag = FALSE;
    return;
  }

  if (! item->text().compare(".."))
  {
    upDirectory();
    return;
  }

  QString s;
  if (item->pixmap())
  {
    // step directory tree down
    s = currentDir.absPath() + "/" + item->text();
    setDirectory(s);
    return;
  }

  getFileSelection(s);
  emit fileOpened(s);
}

void Navigator::setFilter (QString &d)
{
  currentDir.setNameFilter(d);
  updateList();
  // If there is only one item, then select it automatically.
  if (currentDir.count() == 1)
  {
    setCurrentItem(0);
    QString s1;
    getFileSelection(s1);
    QFileInfo info(s1);
    if (info.isFile())
    {
      emit fileOpened(s1);
      selectedFlag = TRUE;
    }
  }
  else
  {
    emit noSelection();
    selectedFlag = FALSE;
  }
}

void Navigator::setId (int d)
{
  id = d;
}

void Navigator::setKeyFlag (bool d)
{
  keyFlag = d;
}

void Navigator::setHome ()
{
  /*QString s = basePath;
  currentDir.setPath(s);
  updateList();
  emit noSelection();
  selectedFlag = FALSE;
  */
  setDirectory(basePath);
}

bool Navigator::isSelected ()
{
  return selectedFlag;
}

void Navigator::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (id, key->state(), key->key(), key->ascii(), key->text());

  doKeyPress(key);
}

void Navigator::doKeyPress (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_Delete:
      key->accept();
      emit keyPress(key->state(), key->key());
      break;
    case Qt::Key_Left: // segfaults if we dont trap this
    case Qt::Key_Right: // segfaults if we dont trap this
      key->accept();
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      key->accept();
      checkDirectory(item(currentItem()));
      break;
    case Qt::Key_Home:
      key->accept();
      setHome();
      Q3ListBox::keyPressEvent(key);
      break;
    default:
      key->accept();
      Q3ListBox::keyPressEvent(key);
      break;
  }
}

/*
void Navigator::highL (QListBoxItem *item)
{
qDebug("highlighted");
}
*/

void Navigator::slotMouseClicked(int btn, Q3ListBoxItem *item, const QPoint &)
{
//qDebug("mouse btn %d clicked", btn);
  if(btn == 1) // left button
  {
    checkDirectory(item);
  }
}
