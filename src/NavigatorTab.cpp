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

#include "NavigatorTab.h"
#include "ChartPage.h"
#include "IndicatorPage.h"
#include "ScannerPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"
#include "GroupPage.h"
#include "ToolBarBtn.h"
#include "RcFile.h"
#include "../pics/indicator.xpm"
#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/portfolio.xpm"
#include "../pics/test.xpm"
#include "../pics/scanner.xpm"
#include <qcursor.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3accel.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QKeyEvent>
#include <Q3VBoxLayout>

NavigatorTab::NavigatorTab (QWidget *w, QMainWindow *mw) : QWidget (w)
{
  loadSettings();

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  vbox->setSpacing(0);
  vbox->setMargin(2);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(vbox);
  hbox->setSpacing(1);
  hbox->setMargin(1);

  bg = new QButtonGroup(this);
  QObject::connect(bg, SIGNAL(buttonPressed(int)), this, SLOT(pressButton(int)));
  bg->setExclusive(TRUE);

  ToolBarBtn *button = new ToolBarBtn(this);
  button->setChecked(true);
  QToolTip::add(button, tr("Workwith Charts"));
  button->setPixmap(plainitem);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->addButton(button, 0);

  button = new ToolBarBtn(this);
  QToolTip::add(button, tr("Workwith Groups"));
  button->setPixmap(dirclosed);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->addButton(button, 1);

  button = new ToolBarBtn(this);
  QToolTip::add(button, tr("Chart Indicators"));
  button->setPixmap(indicator);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->addButton(button, 2);

  button = new ToolBarBtn(this);
  QToolTip::add(button, tr("Workwith Portfolios"));
  button->setPixmap(portfolio);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->addButton(button, 3);

  button = new ToolBarBtn(this);
  QToolTip::add(button, tr("Workwith Backtesting"));
  button->setPixmap(test);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->addButton(button, 4);

  button = new ToolBarBtn(this);
  QToolTip::add(button, tr("Workwith Scanner"));
  button->setPixmap(scanner);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->addButton(button, 5);

  hbox->addStretch(1);

  stack = new Q3WidgetStack(this);
  vbox->addWidget(stack);

  menu = new QMenu;

  positionMenu = new QMenu();
  connect(positionMenu, SIGNAL(aboutToShow ()), this, SLOT(slotTglPosAboutToShow()));
  idMenuLeft = positionMenu->insertItem(tr("Left"), this, SLOT(togglePosition(int)));
  positionMenu->setItemParameter(idMenuLeft, 0);
  idMenuRight = positionMenu->insertItem(tr("Right"), this, SLOT(togglePosition(int)));
  positionMenu->setItemParameter(idMenuRight, 1);
  menu->insertItem (tr("Navigator Position"), positionMenu);

  Q3Accel *a = new Q3Accel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_C, ChartPanelFocus);
  a->insertItem(Qt::CTRL+Qt::Key_G, GroupPanelFocus);
  a->insertItem(Qt::CTRL+Qt::Key_I, IndicatorPanelFocus);
  a->insertItem(Qt::CTRL+Qt::Key_P, PortfolioPanelFocus);
  a->insertItem(Qt::CTRL+Qt::Key_T, TestPanelFocus);
  a->insertItem(Qt::CTRL+Qt::Key_S, ScannerPanelFocus);
}

NavigatorTab::~NavigatorTab ()
{
  saveSettings();
  delete menu;
}

void NavigatorTab::togglePosition (int pos)
{
  position = pos;
  emit signalPositionChanged(pos);
}

void NavigatorTab::saveSettings()
{
  RcFile rcfile;
  rcfile.saveData(RcFile::NavigatorPosition, position);
  rcfile.saveData(RcFile::NavigatorActiveButton, activeButton);
}

void NavigatorTab::loadSettings()
{
  RcFile rcfile;
  rcfile.loadData(RcFile::NavigatorPosition, position);
  rcfile.loadData(RcFile::NavigatorActiveButton, activeButton);
  rcfile.loadData(RcFile::ShowSidePanel,isVisible);
}

int NavigatorTab::getPosition ()
{
  return position;
}

void NavigatorTab::contextMenuEvent (QContextMenuEvent *event)
{
  event->accept();
  menu->exec(QCursor::pos());
}

void NavigatorTab::addWidget (QWidget *w, int id)
{
  stack->addWidget(w, id);
}

void NavigatorTab::buttonPressed (int id)
{
  activeButton = id;
  stack->raiseWidget(id);
  QWidget *w = 0;

  switch (id)
  {
    case 0:
      w = (ChartPage *) stack->widget(id);
      break;
    case 1:
      w = (GroupPage *) stack->widget(id);
      break;
    case 2:
      w = (IndicatorPage *) stack->widget(id);
      break;
    case 3:
      w = (PortfolioPage *) stack->widget(id);
      break;
    case 4:
      w = (TestPage *) stack->widget(id);
      break;
    case 5:
      w = (ScannerPage *) stack->widget(id);
      break;
    default:
      break;
  }

  if (w)
    w->setFocus();
}

void NavigatorTab::pressButton (int id)
{
  if (bg->checkedId() == id)
  {
    buttonPressed(id);
    return;
  }

  bg->button(id)->toggle();
  buttonPressed(id);
}

void NavigatorTab::doKeyPress (QKeyEvent *key)
{
  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_C:
        key->accept();
	pressButton(0);
        break;
      case Qt::Key_G:
        key->accept();
	pressButton(1);
        break;
      case Qt::Key_I:
        key->accept();
	pressButton(2);
        break;
      case Qt::Key_P:
        key->accept();
	pressButton(3);
        break;
      case Qt::Key_T:
        key->accept();
	pressButton(4);
        break;
      case Qt::Key_S:
        key->accept();
	pressButton(5);
        break;
      default:
        break;
    }
  }
  else
    key->ignore();
}

void NavigatorTab::slotAccel (int id)
{
  switch (id)
  {
    case ChartPanelFocus:
      pressButton(id);
      break;
    case GroupPanelFocus:
      pressButton(id);
      break;
    case IndicatorPanelFocus:
      pressButton(id);
      break;
    case PortfolioPanelFocus:
      pressButton(id);
      break;
    case TestPanelFocus:
      pressButton(id);
      break;
    case ScannerPanelFocus:
      pressButton(id);
      break;
    default:
      break;
  }
}

void NavigatorTab::recentTab(QString chartPath)
{
  if (chartPath.startsWith("group/"))
    slotAccel(GroupPanelFocus);
  else
    slotAccel(ChartPanelFocus);
}

void NavigatorTab::slotTglPosAboutToShow()
{
  if(position)
  {
    positionMenu->setItemVisible ( idMenuLeft, TRUE );
    positionMenu->setItemVisible ( idMenuRight, FALSE );

  }
  else
  {
    positionMenu->setItemVisible ( idMenuLeft, FALSE );
    positionMenu->setItemVisible ( idMenuRight, TRUE );
  }
}

void NavigatorTab::init()
{
  pressButton(activeButton);
  emit signaVisibilityChanged(isVisible);

}
