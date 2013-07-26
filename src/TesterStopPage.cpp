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

#include <qlayout.h>
#include <q3vgroupbox.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include "TesterStopPage.h"
#include "Config.h"
#include "IndicatorPlugin.h"
#include "Indicator.h"


TesterStopPage::TesterStopPage (QWidget *p) : QWidget (p)
{
  customShortStopLine = 0;
  customLongStopLine = 0;

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(10);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(vbox);
  hbox->setSpacing(5);

  Q3VGroupBox *gbox = new Q3VGroupBox(tr("Maximum Loss"), this);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  maximumLossCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(maximumLossCheck, SIGNAL(toggled(bool)), this, SLOT(maximumLossToggled(bool)));
  gbox->addSpace(0);

  maximumLossLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  maximumLossShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  validator = new QDoubleValidator(0, 999999, 2, this);

  QLabel *label = new QLabel(tr("Loss %"), gbox);

  maximumLossEdit = new QLineEdit("0", gbox);
  maximumLossEdit->setValidator(validator);

  gbox = new Q3VGroupBox(tr("Profit"), this);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  profitCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(profitCheck, SIGNAL(toggled(bool)), this, SLOT(profitToggled(bool)));
  gbox->addSpace(0);

  profitLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  profitShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  label = new QLabel(tr("Profit %"), gbox);

  profitEdit = new QLineEdit("0", gbox);
  profitEdit->setValidator(validator);

  gbox = new Q3VGroupBox(tr("Trailing"), this);
  gbox->setInsideSpacing(2);
  gbox->setColumns(2);
  hbox->addWidget(gbox);

  trailingCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(trailingCheck, SIGNAL(toggled(bool)), this, SLOT(trailingToggled(bool)));
  gbox->addSpace(0);

  trailingLong = new QCheckBox(tr("Long"), gbox);
  gbox->addSpace(0);

  trailingShort = new QCheckBox(tr("Short"), gbox);
  gbox->addSpace(0);

  label = new QLabel(tr("Loss %"), gbox);

  trailingEdit = new QLineEdit("0", gbox);
  trailingEdit->setValidator(validator);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 2);
  grid->setSpacing(5);

  gbox = new Q3VGroupBox(tr("Custom Long Stop"), this);
  gbox->setInsideSpacing(2);
  gbox->setColumns(1);
  grid->addWidget(gbox, 0, 0);

  customLongStopCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(customLongStopCheck, SIGNAL(toggled(bool)), this, SLOT(customLongStopToggled(bool)));

  customLongStopEdit = new FormulaEdit(gbox, FormulaEdit::Logic);

  gbox = new Q3VGroupBox(tr("Custom Short Stop"), this);
  gbox->setInsideSpacing(2);
  gbox->setColumns(1);
  grid->addWidget(gbox, 0, 1);

  customShortStopCheck = new QCheckBox(tr("Enabled"), gbox);
  connect(customShortStopCheck, SIGNAL(toggled(bool)), this, SLOT(customShortStopToggled(bool)));

  customShortStopEdit = new FormulaEdit(gbox, FormulaEdit::Logic);

  maximumLossToggled(FALSE);
  profitToggled(FALSE);
  trailingToggled(FALSE);
  customLongStopToggled(FALSE);
  customShortStopToggled(FALSE);
}

TesterStopPage::~TesterStopPage ()
{
  if (customLongStopLine)
    delete customLongStopLine;

  if (customShortStopLine)
    delete customShortStopLine;
}

void TesterStopPage::maximumLossToggled (bool status)
{
  if (status)
  {
    maximumLossLong->setEnabled(TRUE);
    maximumLossShort->setEnabled(TRUE);
    maximumLossEdit->setEnabled(TRUE);
  }
  else
  {
    maximumLossLong->setEnabled(FALSE);
    maximumLossShort->setEnabled(FALSE);
    maximumLossEdit->setEnabled(FALSE);
  }
}

void TesterStopPage::profitToggled (bool status)
{
  if (status)
  {
    profitLong->setEnabled(TRUE);
    profitShort->setEnabled(TRUE);
    profitEdit->setEnabled(TRUE);
  }
  else
  {
    profitLong->setEnabled(FALSE);
    profitShort->setEnabled(FALSE);
    profitEdit->setEnabled(FALSE);
  }
}

void TesterStopPage::trailingToggled (bool status)
{
  if (status)
  {
    trailingLong->setEnabled(TRUE);
    trailingShort->setEnabled(TRUE);
    trailingEdit->setEnabled(TRUE);
  }
  else
  {
    trailingLong->setEnabled(FALSE);
    trailingShort->setEnabled(FALSE);
    trailingEdit->setEnabled(FALSE);
  }
}

void TesterStopPage::customShortStopToggled (bool status)
{
  if (status)
    customShortStopEdit->setEnabled(TRUE);
  else
    customShortStopEdit->setEnabled(FALSE);
}

void TesterStopPage::customLongStopToggled (bool status)
{
  if (status)
    customLongStopEdit->setEnabled(TRUE);
  else
    customLongStopEdit->setEnabled(FALSE);
}

bool TesterStopPage::loadCustomShortStop (BarData *recordList)
{
  if (customShortStopLine)
  {
    delete customShortStopLine;
    customShortStopLine = 0;
  }

  if (! customShortStopEdit->getLines() || ! customShortStopCheck->isChecked())
    return FALSE;

  QString s;
  customShortStopEdit->getText(s);
  QStringList l = QStringList::split("\n", s, FALSE);

  QString plugin("CUS");
  Config config;
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  plug->setCustomFunction(l);

  // load the CUS plugin and calculate
  plug->setIndicatorInput(recordList);
  Indicator *i = plug->calculate();
  PlotLine *line = i->getLine(0);
  if (! line)
  {
    qDebug("TesterStopPage::loadCustomShortStop: no PlotLine returned");
    config.closePlugin(plugin);
    delete i;
    return TRUE;
  }

  customShortStopLine = new PlotLine;
  customLongStopLine->copy(line);
  delete i;

  config.closePlugin(plugin);

  return FALSE;
}

bool TesterStopPage::loadCustomLongStop (BarData *recordList)
{
  if (customLongStopLine)
  {
    delete customLongStopLine;
    customLongStopLine = 0;
  }

  if (! customLongStopEdit->getLines() || ! customLongStopCheck->isChecked())
    return FALSE;

  QString s;
  customLongStopEdit->getText(s);
  QStringList l = QStringList::split("\n", s, FALSE);

  QString plugin("CUS");
  Config config;
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return TRUE;
  }

  plug->setCustomFunction(l);

  // load the CUS plugin and calculate
  plug->setIndicatorInput(recordList);
  Indicator *i = plug->calculate();
  PlotLine *line = i->getLine(0);
  if (! line)
  {
    qDebug("Tester::loadCustomShortStop: no PlotLine returned");
    config.closePlugin(plugin);
    delete i;
    return TRUE;
  }

  customLongStopLine = new PlotLine;
  customLongStopLine->copy(line);
  delete i;

  config.closePlugin(plugin);

  return FALSE;
}

void TesterStopPage::loadCustomStopRule (QString &ruleName)
{
  Config config;
  QString s;
  config.getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customLongStop");
  QFile f(s);
  if (! f.open(QIODevice::ReadOnly))
    return;
  Q3TextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    customLongStopEdit->setLine(s);
  }

  f.close();

  config.getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customShortStop");
  f.setName(s);
  if (! f.open(QIODevice::ReadOnly))
    return;

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    customShortStopEdit->setLine(s);
  }

  f.close();
}

void TesterStopPage::saveCustomStopRule (QString &ruleName)
{
  Config config;
  QString s;
  config.getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customShortStop");
  QFile f(s);
  if (! f.open(QIODevice::WriteOnly))
    return;
  Q3TextStream stream(&f);

  customShortStopEdit->getText(s);
  stream << s << "\n";

  f.close();

  config.getData(Config::TestPath, s);
  s.append("/" + ruleName + "/customLongStop");
  f.setName(s);
  if (! f.open(QIODevice::WriteOnly))
    return;

  customLongStopEdit->getText(s);
  stream << s << "\n";

  f.close();
}

bool TesterStopPage::maximumLoss (bool flag, double enterPrice, double exitPrice)
{
  if (! maximumLossCheck->isChecked())
    return FALSE;

  if (! flag)
  {
    double t = enterPrice * (1 - (maximumLossEdit->text().toDouble() / 100));
    if (exitPrice <= t)
      return TRUE;
  }
  else
  {
    double t = enterPrice * (1 + (maximumLossEdit->text().toDouble() / 100));
    if (exitPrice >= t)
      return TRUE;
  }

  return FALSE;
}

bool TesterStopPage::profit (bool flag, double enterPrice, double exitPrice)
{
  if (! profitCheck->isChecked())
    return FALSE;

  if (! flag)
  {
    double t = enterPrice * (1 + (profitEdit->text().toDouble() / 100));
    if (exitPrice >= t)
      return TRUE;
  }
  else
  {
    double t = enterPrice * (1 - (profitEdit->text().toDouble() / 100));
    if (exitPrice <= t)
      return TRUE;
  }

  return FALSE;
}

bool TesterStopPage::trailing (bool flag, double exitPrice)
{
  if (! trailingCheck->isChecked())
    return FALSE;

  if (! flag)
  {
    if (exitPrice < trailingHigh)
      trailingHigh = exitPrice;

    double t = ((exitPrice - trailingHigh) / trailingHigh) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= trailingEdit->text().toDouble())
        return TRUE;
    }
  }
  else
  {
    if (exitPrice < trailingLow)
      trailingLow = exitPrice;

    double t = ((trailingLow - exitPrice) / trailingLow) * 100;
    if (t < 0)
    {
      t = -t;
      if (t >= trailingEdit->text().toDouble())
        return TRUE;
    }
  }

  return FALSE;
}

bool TesterStopPage::customStop (bool flag, int index)
{
  if (! flag)
  {
    if (customLongStopCheck->isChecked() && customLongStopEdit->getLines() && customLongStopLine)
    {
      int i = customLongStopLine->getSize() - index;
      if (i > -1)
      {
        if (customLongStopLine->getData(i) == 1)
          return TRUE;
      }
    }
  }
  else
  {
    if (customShortStopCheck->isChecked() && customShortStopEdit->getLines() && customShortStopLine)
    {
      int i = customShortStopLine->getSize() - index;
      if (i > -1)
      {
        if (customShortStopLine->getData(i) == 1)
          return TRUE;
      }
    }
  }

  return FALSE;
}

bool TesterStopPage::getMaximumLossCheck ()
{
  return maximumLossCheck->isChecked();
}

void TesterStopPage::setMaximumLossCheck (bool d)
{
  maximumLossCheck->setChecked(d);
}

bool TesterStopPage::getMaximumLossLong ()
{
  return maximumLossLong->isChecked();
}

void TesterStopPage::setMaximumLossLong (bool d)
{
  maximumLossLong->setChecked(d);
}

bool TesterStopPage::getMaximumLossShort ()
{
  return maximumLossShort->isChecked();
}

void TesterStopPage::setMaximumLossShort (bool d)
{
  maximumLossShort->setChecked(d);
}

QString TesterStopPage::getMaximumLossEdit ()
{
  return maximumLossEdit->text();
}

void TesterStopPage::setMaximumLossEdit (QString d)
{
  maximumLossEdit->setText(d);
}

bool TesterStopPage::getProfitCheck ()
{
  return profitCheck->isChecked();
}

void TesterStopPage::setProfitCheck (bool d)
{
  profitCheck->setChecked(d);
}

bool TesterStopPage::getProfitLong ()
{
  return profitLong->isChecked();
}

void TesterStopPage::setProfitLong (bool d)
{
  profitLong->setChecked(d);
}

bool TesterStopPage::getProfitShort ()
{
  return profitShort->isChecked();
}

void TesterStopPage::setProfitShort (bool d)
{
  profitShort->setChecked(d);
}

QString TesterStopPage::getProfitEdit ()
{
  return profitEdit->text();
}

void TesterStopPage::setProfitEdit (QString d)
{
  profitEdit->setText(d);
}

//

bool TesterStopPage::getTrailingCheck ()
{
  return trailingCheck->isChecked();
}

void TesterStopPage::setTrailingCheck (bool d)
{
  trailingCheck->setChecked(d);
}

bool TesterStopPage::getTrailingLong ()
{
  return trailingLong->isChecked();
}

void TesterStopPage::setTrailingLong (bool d)
{
  trailingLong->setChecked(d);
}

bool TesterStopPage::getTrailingShort ()
{
  return trailingShort->isChecked();
}

void TesterStopPage::setTrailingShort (bool d)
{
  trailingShort->setChecked(d);
}

QString TesterStopPage::getTrailingEdit ()
{
  return trailingEdit->text();
}

void TesterStopPage::setTrailingEdit (QString d)
{
  trailingEdit->setText(d);
}

void TesterStopPage::setTrailingHigh (double d)
{
  trailingHigh = d;
}
