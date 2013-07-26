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

#ifndef EXTRATOOLBAR_HPP
#define EXTRATOOLBAR_HPP

#include <qstring.h>
#include <qtoolbar.h>
#include <qmainwindow.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qaction.h>

#include "RcFile.h"

class ExtraToolbar : public QToolBar
{
  Q_OBJECT

  signals:
    void fileSelected (QString);
    void recentTab (QString);
    void signalSetGroupNavItem (QString, QString);
    void signalSetChartNavItem (QString, QString);

  public:

    ExtraToolbar(QMainWindow *);
    ~ExtraToolbar();
    void saveSettings ();

  public slots:
    void slotSetButtonView ();
    void slotAddRecentChart (QString);
    void slotRemoveRecentCharts (QStringList);

  private:
    QComboBox *recentCombo;
    QAction *recentComboAction;
    RcFile rcfile;
    void sortRecentCharts (QString);

  private slots:
    void slotRecentChartSelected(int);
};

#endif
