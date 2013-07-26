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

#ifndef PORTFOLIOPAGE_HPP
#define PORTFOLIOPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qmenu.h>
#include <q3listbox.h>
//Added by qt3to4:
#include <QKeyEvent>
#include "Config.h"
#include "DBIndex.h"

class PortfolioPage : public Q3ListBox
{
  Q_OBJECT

  public:

    enum HotKey
    {
      NewPortfolio,
      DeletePortfolio,
      RenamePortfolio,
      OpenPortfolio,
      Help
    };

    PortfolioPage (QWidget *, DBIndex *);
    ~PortfolioPage ();

  public slots:
    void openPortfolio ();
    void openPortfolio (QString);
    void renamePortfolio ();
    void newPortfolio ();
    void deletePortfolio ();
    void portfolioSelected (const QString &);
    void rightClick (Q3ListBoxItem *);
    void slotHelp ();
    void doubleClick (Q3ListBoxItem *);
    void updateList ();
    void doKeyPress (QKeyEvent *);
    void slotAccel (int);

  private:
    virtual void keyPressEvent (QKeyEvent *);

    Config config;
    QMenu *menu;
    DBIndex *chartIndex;
};

#endif
