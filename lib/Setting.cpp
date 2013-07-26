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

#include "Setting.h"
#include <qobject.h>

Setting::Setting ()
{
  dict.setAutoDelete(TRUE);
}

Setting::~Setting ()
{
}

void Setting::getData (QString &k, QString &d)
{
  d.truncate(0);
  QString *s = dict[k];
  if (s)
    d = s->left(s->length());
}

double Setting::getDouble (QString &k)
{
  QString *s = dict[k];
  if (s)
    return s->toDouble();
  else
    return 0;
}

int Setting::getInt (QString &k)
{
  QString *s = dict[k];
  if (s)
    return s->toInt();
  else
    return 0;
}

void Setting::setData (QString &k, QString &d)
{
  dict.replace(k, new QString(d));
}

void Setting::getKeyList (QStringList &l)
{
  l.clear();
  Q3DictIterator<QString> it(dict);
  for (; it.current(); ++it)
    l.append(it.currentKey());
}

void Setting::remove (QString &k)
{
  dict.remove(k);
}

void Setting::getString (QString &s)
{
  s.truncate(0);
  QStringList l;
  Q3DictIterator<QString> it(dict);
  for (; it.current(); ++it)
  {
    QString *s = it.current();
    l.append(it.currentKey() + "=" + s->left(s->length()));
  }
  s = l.join("|");
}

void Setting::parse (QString &d)
{
  dict.clear();

  QStringList l = QStringList::split("|", d, FALSE);

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = QStringList::split("=", l[loop], FALSE);
    if (l2.count() > 1)
      dict.replace(l2[0], new QString(l2[1]));
  }
}

void Setting::clear ()
{
  dict.clear();
}

int Setting::count ()
{
  return (int) dict.count();
}

void Setting::copy (Setting *r)
{
  QString k, d;
  Q3DictIterator<QString> it(dict);
  for (; it.current(); ++it)
  {
    QString *s = it.current();
    k = it.currentKey();
    d = s->left(s->length());
    r->setData(k, d);
  }
}
