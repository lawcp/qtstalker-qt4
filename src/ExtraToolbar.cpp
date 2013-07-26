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

#include "ExtraToolbar.h"
#include "PrefDialog.h"
#include <qtooltip.h>
#include <qstringlist.h>

// FIXME: Why are sometimes two or more entries ticked in recentCombo.

// FIXME: Make maxItems configurable.

ExtraToolbar::ExtraToolbar (QMainWindow *mw) : QToolBar ("extraToolbar", mw)
{
  // Recent viewed charts.
  recentCombo = new QComboBox(this);
  recentComboAction = addWidget(recentCombo);
  QToolTip::add(recentCombo, tr("Recent charts"));

// FIXME: Having trouble getting ComboBox to dynamically set its width
// FIXME: Perhaps use setSizeAdjustPolicy(QComboBox::AdjustToContents) when Qt4.
  recentCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  //recentCombo->setFixedWidth(260);

  connect(recentCombo, SIGNAL(activated(int)), this, SLOT(slotRecentChartSelected(int)));

  QString s;
  rcfile.loadData(RcFile::RecentChartsList, s);
  QStringList sl = QStringList::split(",", s);
  if (sl.count() > 0)
  {
    for (int loop = 0; loop < (int) sl.count(); loop++)
      recentCombo->insertItem(sl[loop]);
  }

  bool tb;
  rcfile.loadData(RcFile::ShowExtraToolbar, tb);
  if (!tb)
    hide();

// FIXME: Is font set correctly to the appFont?
/*
  QFont myFont = recentCombo->font();
  QString myFontS = myFont.toString();
  qDebug("Combo font=%s", myFontS.latin1());
*/
}

ExtraToolbar::~ExtraToolbar ()
{
}

void ExtraToolbar::saveSettings ()
{
  QString s;
  for (int loop = 0; loop < recentCombo->count(); loop++)
  {
    s += recentCombo->text(loop) + ",";
  }
  s.remove(s.length()-1,1); // Remove trailing comma.
  rcfile.saveData(RcFile::RecentChartsList, s);
}

void ExtraToolbar::slotSetButtonView ()
{
  bool tb;

  rcfile.loadData(RcFile::ShowRecentCharts, tb);
  if (tb) recentComboAction->setVisible(true);
  else recentComboAction->setVisible(false);
}

void ExtraToolbar::slotRecentChartSelected (int)
{
  QString chartSelected = recentCombo->currentText();
  sortRecentCharts(chartSelected);
  QString s;
  rcfile.loadData(RcFile::Home, s);
  s += "/" + chartSelected;

  QString chartName = chartSelected.section( '/', -1 );
  QString chartPath = chartSelected;
  int i = chartPath.findRev("/" + chartName, -1, TRUE);
  chartPath.remove(i, chartName.length()+1);

  QString chartDir;
  rcfile.loadData(RcFile::Home, chartDir);
  chartDir += "/" + chartPath;

  emit recentTab(chartSelected);
  if (chartPath.startsWith("group/"))
    emit signalSetGroupNavItem(chartDir, chartName);
  else
    emit signalSetChartNavItem(chartDir, chartName);

  emit fileSelected(s);
}

void ExtraToolbar::slotAddRecentChart (QString selection)
{
  QString s;
  rcfile.loadData(RcFile::Home, s);
  s += "/";
  selection.remove(s); // relative Group or Chart path, and the chart name
  sortRecentCharts(selection);
  if (recentCombo->count() > 10)
    recentCombo->removeItem(10);
}

void ExtraToolbar::slotRemoveRecentCharts (QStringList l)
{
  QString s, sl;
  rcfile.loadData(RcFile::Home, s);
  s += "/";
  for (int i = 0; i < (int) l.count(); i++)
  {
    sl = l[i];
    sl.remove(s);
    for (int j = 0; j < recentCombo->count(); j++)
    {
      if (!QString::compare(recentCombo->text(j), sl))
        recentCombo->removeItem(j);
    }
  }
}

void ExtraToolbar::sortRecentCharts (QString s)
{
  // If the item is already in the list then remove it, then insert at top
  for (int loop = 0; loop < recentCombo->count(); loop++)
  {
    if (!QString::compare(recentCombo->text(loop), s))
      recentCombo->removeItem(loop);
  }
  recentCombo->insertItem(s, 0);
  recentCombo->setCurrentItem(0);
}
