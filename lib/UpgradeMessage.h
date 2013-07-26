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

#ifndef UPGRADEMESSAGE_HPP
#define UPGRADEMESSAGE_HPP

#include <q3tabdialog.h>
#include <qcheckbox.h>
#include <q3progressbar.h>
#include "Setting.h"
#include "DbPlugin.h"
#include "DBIndex.h"
#include "DBIndexItem.h"

class UpgradeMessage : public Q3TabDialog
{
  Q_OBJECT

  public:

    enum UpgradeMessageVersion
    {
      V031,
      V034
    };

    UpgradeMessage (int);
    ~UpgradeMessage ();
    bool getStatus ();
    void createPage031 ();
    void createPage034 ();
    bool createChart (QString &);
    bool createDir (QString &);
    void copyFiles ();
    void correctPathFiles (QStringList &);
    void saveHeaderData (DbPlugin &db, QString &k, QString &d, QString &, DBIndexItem &);

  public slots:
    void convert034 ();

  private:
    QCheckBox *check;
    Q3ProgressBar *progBar;
    DBIndex index;
};

#endif

// remove
