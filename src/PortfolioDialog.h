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

#ifndef PORTFOLIODIALOG_HPP
#define PORTFOLIODIALOG_HPP

#include <q3listview.h>
#include <qstring.h>
#include <q3tabdialog.h>
#include <qlabel.h>
#include <qpoint.h>
#include "Config.h"
#include "Toolbar.h"
#include "DBIndex.h"

class PortfolioDialog : public Q3TabDialog
{
  Q_OBJECT

  public:
    PortfolioDialog (QString, DBIndex *);
    ~PortfolioDialog ();
    void updatePortfolio ();
    double futuresProfit (QString &, double);
    void updatePortfolioItems ();

  public slots:
    void modifyItem ();
    void addItem ();
    void deleteItem ();
    void savePortfolio ();
    void buttonStatus (Q3ListViewItem *);
    void slotHelp ();
    void itemDoubleClicked (Q3ListViewItem *, const QPoint &, int);

  private:
    Q3ListView *plist;
    Q3ListViewItem *item;
    QString portfolio;
    Toolbar *toolbar;
    Config config;
    QLabel *balance;
    DBIndex *index;
};

#endif
