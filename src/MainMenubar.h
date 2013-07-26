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

#ifndef MAINMENUBAR_HPP
#define MAINMENUBAR_HPP

#include <qstring.h>
#include <qmenubar.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <q3intdict.h>
#include <qaction.h>
//Added by qt3to4:
#include <QKeyEvent>

class MainMenubar : public QMenuBar
{
  Q_OBJECT

  signals:
    void signalExit ();
    void signalGrid (bool);
    void signalScale (bool);
    void signalDraw (bool);
    void signalCrosshairs (bool);
    void signalSidePanel (bool);
    void signalNewIndicator();
    void signalOptions();
    void signalQuotes();
    void signalPaperTrade (bool);
    void signalAdvancePaperTrade ();

  public:

    enum MenuAction
    {
      Exit,
      NewIndicator,
      Options,
      Grid,
      Quotes,
      DataWindow,
      About,
      ScaleToScreen,
      SidePanel,
      DrawMode,
      Crosshairs,
      Help,
      PaperTrade,
      AdvancePaperTrade,
      IndicatorSummary
    };

    MainMenubar(QMainWindow *);
    ~MainMenubar();
    void createMenus ();
    QAction * getAction (int);
    bool getStatus (int);
    void setStatus (int, bool);
    void saveSettings ();

  public slots:
    void slotAccel (int);
    void doKeyPress (QKeyEvent *);

  private:
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *toolMenu;
    QMenu *helpMenu;
    Q3IntDict<QAction> actions;
};

#endif
