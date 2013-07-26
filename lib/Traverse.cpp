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

#include "Traverse.h"
#include <qdir.h>
#include <qfileinfo.h>

Traverse::Traverse (Traverse::Type t)
{
  type = t;
}

Traverse::~Traverse ()
{
}

void Traverse::traverse (QString dirname)
{
  QDir dir(dirname);
  dir.setFilter(QDir::Dirs|QDir::Files);

  QFileInfoList fileinfolist = dir.entryInfoList();
  int ret = -1;
  for(QFileInfoListIterator it=fileinfolist.begin();ret && it!=fileinfolist.end();++it)
  {
    if(it->fileName() == "." || it->fileName() == "..")
    {
      continue;
    }

    if(it->isDir() && it->isReadable())
    {
      if (type == Dir)
        list.append(it->absFilePath());
      traverse(it->absFilePath());
    }
    else
    {
      if (type == File)
        list.append(it->absFilePath());
    }

  }
}

void Traverse::getList (QStringList &l)
{
  l = list;
}

void Traverse::clear ()
{
  list.clear();
}
