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

#include "FileButton.h"
#include <q3filedialog.h>
#include <qfileinfo.h>

FileButton::FileButton (QWidget *w, QStringList &l, QString &p) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fileDialog()));
  setMaximumHeight(25);
  setToggleButton(FALSE);
  fileList = l;

  path = QDir::homeDirPath();
  if (p.length())
    path = p;

  updateButtonText();
}

FileButton::~FileButton ()
{
}

void FileButton::getFile (QStringList &l)
{
  l.clear();
  l = fileList;
}

void FileButton::setFile (QStringList &l)
{
  fileList = l;
  updateButtonText();
}

void FileButton::fileDialog ()
{
  if (fileList.count())
  {
    QFileInfo fi(fileList[0]);
    path = fi.dirPath(TRUE);
  }

  QStringList l = Q3FileDialog::getOpenFileNames("*", path, this, "file dialog");
  if (l.count())
    fileList = l;

  updateButtonText();
}

void FileButton::updateButtonText ()
{
  QString s = QString::number(fileList.count());
  s.append(" ");
  s.append(tr("Files"));
  setText(s);
}
