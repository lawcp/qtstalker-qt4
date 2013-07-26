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

#ifndef NAVIGATORTAB_HPP
#define NAVIGATORTAB_HPP

#include <qmenu.h>
#include <q3widgetstack.h>
#include <qwidget.h>
#include <qbuttongroup.h>
#include <qmainwindow.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QContextMenuEvent>

class NavigatorTab : public QWidget
{
  Q_OBJECT

  signals:
    void signalPositionChanged (int);
    void signaVisibilityChanged (bool);

  public:

    enum HotKey
    {
      ChartPanelFocus,
      GroupPanelFocus,
      IndicatorPanelFocus,
      PortfolioPanelFocus,
      TestPanelFocus,
      ScannerPanelFocus
    };

    NavigatorTab (QWidget *, QMainWindow *);
    ~NavigatorTab ();
    void loadSettings();
    void saveSettings();
    int getPosition ();
    void addWidget (QWidget *w, int);
    void init ();

  public slots:
    void togglePosition (int);
    void buttonPressed (int);
    void pressButton (int);
    void doKeyPress (QKeyEvent *);
    void slotAccel (int);
    void recentTab (QString);

  protected:
    virtual void contextMenuEvent (QContextMenuEvent *);

  protected slots:
    void slotTglPosAboutToShow();

  private:
    int position;
    QMenu *menu;
    QMenu *positionMenu;
    int idMenuLeft;
    int idMenuRight;
    Q3WidgetStack *stack;
    QButtonGroup *bg;
    int activeButton;
    bool isVisible;
};

#endif
