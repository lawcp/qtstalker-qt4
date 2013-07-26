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

#include "MainMenubar.h"
#include <q3accel.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include "../pics/finished.xpm"
#include "../pics/gridicon.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/quotes.xpm"
#include "../pics/configure.xpm"
#include "../pics/scaletoscreen.xpm"
#include "../pics/nav.xpm"
#include "../pics/co.xpm"
#include "../pics/help.xpm"
#include "../pics/qtstalker.xpm"
#include "../pics/crosshair.xpm"
#include "../pics/papertrade.xpm"
#include "RcFile.h"


MainMenubar::MainMenubar (QMainWindow *mw) : QMenuBar (mw, "mainMenubar")
{
  actions.setAutoDelete(FALSE);

  RcFile rcfile;

  QPixmap icon(finished);
  QAction *action  = new QAction(this, "actionExit");
  action->setMenuText(tr("E&xit"));
  action->setIconSet(icon);
  action->setAccel(Qt::CTRL+Qt::Key_Q);
  action->setStatusTip(tr("Quit Qtstalker (Ctrl+Q)"));
  action->setToolTip(tr("Quit Qtstalker (Ctrl+Q)"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalExit()));
  actions.replace(Exit, action);

  icon = configure;
  action = new QAction(this, "actionOptions");
  action->setMenuText(tr("Edit &Preferences"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Modify user preferences (Ctrl+0)"));
  action->setToolTip(tr("Modify user preferences (Ctrl+0)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotOptions()));
  actions.replace(Options, action);

  icon = nav;
  bool b;
  rcfile.loadData(RcFile::ShowSidePanel, b);
  action = new QAction(this, "actionPanel");
  action->setMenuText(tr("Side &Panel"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle side-panel view (Ctrl+1)"));
  action->setToolTip(tr("Toggle side-panel view (Ctrl+1)"));
  action->setToggleAction(TRUE);
  action->setOn(b);
  connect(action, SIGNAL(toggled(bool)), mw, SLOT(slotHideNav(bool)));
  actions.replace(SidePanel, action);

  icon = gridicon;
  rcfile.loadData(RcFile::Grid, b);
  action = new QAction(this, "actionGrid");
  action->setMenuText(tr("Chart &Grid"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle chart grid (Ctrl+2)"));
  action->setToolTip(tr("Toggle chart grid (Ctrl+2)"));
  action->setToggleAction(TRUE);
  action->setOn(b);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalGrid(bool)));
  actions.replace(Grid, action);

  icon = scaletoscreen;
  rcfile.loadData(RcFile::ScaleToScreen, b);
  action = new QAction(this, "actionScale");
  action->setMenuText(tr("&Scale To Screen"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Scale chart to current screen data (Ctrl+3)"));
  action->setToolTip(tr("Scale chart to current screen data (Ctrl+3)"));
  action->setToggleAction(TRUE);
  action->setOn(b);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalScale(bool)));
  actions.replace(ScaleToScreen, action);

  icon = crosshair;
  rcfile.loadData(RcFile::Crosshairs, b);
  action = new QAction(this, "actionCrosshairs");
  action->setMenuText(tr("Toggle &Crosshairs"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle crosshairs (Ctrl+4)"));
  action->setToolTip(tr("Toggle crosshairs (Ctrl+4)"));
  action->setToggleAction(TRUE);
  action->setOn(b);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalCrosshairs(bool)));
  actions.replace(Crosshairs, action);

  icon = co;
  rcfile.loadData(RcFile::DrawMode, b);
  action = new QAction(this, "actionDraw");
  action->setMenuText(tr("Toggle &Draw Mode"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle drawing mode (Ctrl+5)"));
  action->setToolTip(tr("Toggle drawing mode (Ctrl+5)"));
  action->setToggleAction(TRUE);
  action->setOn(b);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalDraw(bool)));
  actions.replace(DrawMode, action);

  icon = papertrade;
  rcfile.loadData(RcFile::PaperTradeMode, b);
  action = new QAction(this, "actionPaperTrade");
  action->setMenuText(tr("Toggle &Trade Mode"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle paper-trade mode"));
  action->setToolTip(tr("Toggle paper-trade mode"));
  action->setToggleAction(TRUE);
  action->setOn(b);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalPaperTrade(bool)));
  actions.replace(PaperTrade, action);

  icon = indicator;
  action = new QAction(this, "actionNewIndicator");
  action->setMenuText(tr("New &Indicator"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Add new indicator (Ctrl+7)"));
  action->setToolTip(tr("Add new indicator (Ctrl+7)"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalNewIndicator()));
  actions.replace(NewIndicator, action);

  icon = datawindow;
  action = new QAction(this, "actionDataWindow");
  action->setMenuText(tr("&Data Window"));
  action->setIconSet(icon);
  action->setAccel(Qt::ALT+Qt::Key_1);
  action->setStatusTip(tr("Show data window (Alt+1)"));
  action->setToolTip(tr("Show data window (Alt+1)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotDataWindow()));
  actions.replace(DataWindow, action);

  icon = quotes;
  action = new QAction(this, "actionQuote");
  action->setMenuText(tr("Load Quotes"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Load Quotes (Ctrl+Y)"));
  action->setToolTip(tr("Load Quotes (Ctrl+Y)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotQuotes()));
  actions.replace(Quotes, action);

  icon = qtstalker;
  action = new QAction(this, "actionAbout");
  action->setMenuText(tr("&About"));
  action->setIconSet(icon);
  action->setStatusTip(tr("About Qtstalker."));
  action->setToolTip(tr("About Qtstalker."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotAbout()));
  actions.replace(About, action);

  icon = help;
  action = new QAction(this, "actionHelp");
  action->setMenuText(tr("&Help"));
  action->setIconSet(icon);
  action->setAccel(Qt::Key_F1);
  action->setStatusTip(tr("Display Help (F1)"));
  action->setToolTip(tr("Display Help (F1)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotHelp()));
  actions.replace(Help, action);

  action = new QAction(this, "actionAdvancePaperTrade");
  action->setAccel(Qt::CTRL+Qt::Key_Right);
  connect(action, SIGNAL(activated()), this, SIGNAL(signalAdvancePaperTrade()));
  actions.replace(AdvancePaperTrade, action);

  action = new QAction(this, "actionIndicatorSummary");
  action->setMenuText(tr("Indicator Summary"));
  action->setStatusTip(tr("Indicator Summary"));
  action->setToolTip(tr("Indicator Summary"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotIndicatorSummary()));
  actions.replace(IndicatorSummary, action);

  Q3Accel *a = new Q3Accel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_0, Options);
  a->insertItem(Qt::CTRL+Qt::Key_1, SidePanel);
  a->insertItem(Qt::CTRL+Qt::Key_2, Grid);
  a->insertItem(Qt::CTRL+Qt::Key_3, ScaleToScreen);
  a->insertItem(Qt::CTRL+Qt::Key_4, Crosshairs);
  a->insertItem(Qt::CTRL+Qt::Key_5, DrawMode);
  a->insertItem(Qt::CTRL+Qt::Key_7, NewIndicator);
  a->insertItem(Qt::CTRL+Qt::Key_Y, Quotes);
  a->insertItem(Qt::CTRL+Qt::Key_Right, AdvancePaperTrade);
  a->insertItem(Qt::CTRL+Qt::Key_Escape, 8);

  createMenus();

  rcfile.loadData(RcFile::ShowMenuBar, b);
  if (!b)
    hide();
}

MainMenubar::~MainMenubar ()
{
}

void MainMenubar::createMenus ()
{
  fileMenu = new QMenu();
  actions[Exit]->addTo(fileMenu);

  editMenu = new QMenu();
  actions[NewIndicator]->addTo(editMenu);
  actions[Options]->addTo(editMenu);

  viewMenu = new QMenu();
  viewMenu->setCheckable(true);
  actions[Grid]->addTo(viewMenu);
  actions[ScaleToScreen]->addTo(viewMenu);
  actions[SidePanel]->addTo(viewMenu);
  actions[Crosshairs]->addTo(viewMenu);
  actions[DrawMode]->addTo(viewMenu);
  actions[PaperTrade]->addTo(viewMenu);

  toolMenu = new QMenu();
  actions[DataWindow]->addTo(toolMenu);
  actions[IndicatorSummary]->addTo(toolMenu);
  actions[Quotes]->addTo(toolMenu);

  helpMenu = new QMenu();
  actions[Help]->addTo(helpMenu);
  actions[About]->addTo(helpMenu);

  insertItem(tr("&File"), fileMenu);
  insertItem(tr("&Edit"), editMenu);
  insertItem(tr("&View"), viewMenu);
  insertItem(tr("&Tools"), toolMenu);
  insertSeparator();
  insertItem(tr("&Help"), helpMenu);
}

QAction * MainMenubar::getAction (int d)
{
  return actions[d];
}

bool MainMenubar::getStatus (int d)
{
  return actions[d]->isOn();
}

void MainMenubar::setStatus (int d, bool f)
{
  actions[d]->setOn(f);
}

void MainMenubar::saveSettings ()
{
  RcFile rcfile;
  rcfile.saveData(RcFile::Crosshairs, !getStatus(DrawMode));
  rcfile.saveData(RcFile::DrawMode, getStatus(DrawMode));
  rcfile.saveData(RcFile::Grid, getStatus(Grid));
  rcfile.saveData(RcFile::PaperTradeMode, getStatus(PaperTrade));
  rcfile.saveData(RcFile::ScaleToScreen, getStatus(ScaleToScreen));
  rcfile.saveData(RcFile::ShowSidePanel, getStatus(SidePanel));
}

void MainMenubar::slotAccel (int id)
{
  switch (id)
  {
    case Options:
      emit signalOptions();
      break;
    case SidePanel:
      getAction(SidePanel)->toggle();
      break;
    case Grid:
      getAction(Grid)->toggle();
      break;
    case ScaleToScreen:
      getAction(ScaleToScreen)->toggle();
      break;
    case Crosshairs:
      getAction(Crosshairs)->toggle();
      break;
    case DrawMode:
      getAction(DrawMode)->toggle();
      break;
    case AdvancePaperTrade:
      emit signalAdvancePaperTrade();
      break;
    case NewIndicator:
      emit signalNewIndicator();
      break;
    case Quotes:
      emit signalQuotes();
      break;
    default:
      break;
  }
}

void MainMenubar::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_0:
	slotAccel(Options);
        break;
      case Qt::Key_1:
	slotAccel(SidePanel);
        break;
      case Qt::Key_2:
	slotAccel(Grid);
        break;
      case Qt::Key_3:
	slotAccel(ScaleToScreen);
        break;
      case Qt::Key_4:
	slotAccel(Crosshairs);
        break;
      case Qt::Key_5:
	slotAccel(DrawMode);
        break;
      case Qt::Key_7:
	slotAccel(NewIndicator);
        break;
      case Qt::Key_Y:
        slotAccel(Quotes);
        break;
      case Qt::Key_Right:
        slotAccel(AdvancePaperTrade);
        break;
      default:
        break;
    }
  }
}
