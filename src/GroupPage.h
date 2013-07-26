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

#ifndef GROUPPAGE_HPP
#define GROUPPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qmenu.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QKeyEvent>
#include "Navigator.h"
#include "RcFile.h"

class GroupPage : public QWidget
{
  Q_OBJECT

  signals:
    void fileSelected (QString);
    void addRecentChart (QString);
    void removeRecentCharts (QStringList);

  public:

    enum HotKey
    {
      NewGroup,
      AddGroupItem,
      DeleteGroupItem,
      DeleteGroup,
      RenameGroup,
      Help,
      BrowseChart,
      DeleteChart
    };

    GroupPage (QWidget *);
    ~GroupPage ();
    void setFocus ();

  public slots:
    void newGroup ();
    void addGroupItem ();
    void deleteGroupItem ();
    void deleteGroup ();
    void renameGroup ();
    void groupSelected (QString);
    void groupNoSelection ();
    void setGroupNavItem (QString, QString);
    void rightClick (Q3ListBoxItem *);
    void slotHelp ();
    void doKeyPress (QKeyEvent *);
    void slotAccel (int);
    void chartOpened (QString);
    void refreshList ();
    void browseChart ();
    void deleteChart ();
    void addChartToGroup (QString);

  protected:
    Navigator *nav;
    RcFile rcfile;
    QMenu *menu;
    QLineEdit *group;
};

#endif
