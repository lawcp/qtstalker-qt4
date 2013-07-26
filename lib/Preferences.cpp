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

#include "Preferences.h"
#include "HelpWindow.h"
#include "RcFile.h"

#include <qlabel.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>


Preferences::Preferences(QWidget *w) : Q3TabDialog(w, "Preferences", FALSE, Qt::WDestructiveClose)
{
  helpFile = "preferences.html";

  setCaption(tr("Edit Prefs"));

  loadSettings();
  createGeneralPage();
  createColorPage();
  createFontPage();
  createMTPage(); // main tool bar
  createCTPage(); // chart tool bar
  createETPage(); // extra tool bar

  setApplyButton(tr("&Apply"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(slotSave()));

  setCancelButton(tr("&Ok"));
  //connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(exitDialog()));

  setOkButton(QString::null);

  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(slotHelp()));
  resize(sz);

}

Preferences::~Preferences()
{
  RcFile rcfile;
  rcfile.saveSize(RcFile::PrefDlgWindowSize, size());
}

void Preferences::loadSettings()
{
  RcFile rcfile;

  rcfile.loadData(RcFile::ShowMenuBar, menubar);
  rcfile.loadData(RcFile::ShowExtraToolbar, extraToolbar);
  rcfile.loadData(RcFile::PSButton, ps1Button, 1);
  rcfile.loadData(RcFile::PSButton, ps2Button, 2);
  rcfile.loadData(RcFile::PSButton, ps3Button, 3);
  rcfile.loadColor(RcFile::BackgroundColor, backgroundColor);
  rcfile.loadColor(RcFile::BorderColor, borderColor);
  rcfile.loadColor(RcFile::GridColor, gridColor);
  rcfile.loadFont(RcFile::PlotFont, plotFont);
  rcfile.loadFont(RcFile::AppFont, appFont);
  rcfile.loadSize(RcFile::PrefDlgWindowSize, sz);
}

void Preferences::createGeneralPage()
{
  // general parms page

  QWidget *w = new QWidget(this);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 3);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1); // stretch 2nd col a little
  grid->setColStretch(2, 2); // stretch outer right col more

  vbox->insertStretch(-1, 1);

  // menubar checkbox
  QLabel *label = new QLabel(tr("Main Menubar"), w);
  grid->addWidget(label, 0, 0);

  menubarCheck = new QCheckBox(w);
  menubarCheck->setChecked(menubar);
  connect(menubarCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(menubarCheck, 0, 1);

  // extraToolbar checkbox
  label = new QLabel(tr("Extra Toolbar"), w);
  grid->addWidget(label, 1, 0);

  extraToolbarCheck = new QCheckBox(w);
  extraToolbarCheck->setChecked(extraToolbar);
  connect(extraToolbarCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(extraToolbarCheck, 1, 1);

  // bar spacing 1
  label = new QLabel(tr("Bar Spacing 1"), w);
  grid->addWidget(label, 2, 0);

  bs1Spinner = new QSpinBox(2, 99, 1, w);
  bs1Spinner->setValue(ps1Button);
  connect(bs1Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs1Spinner, 2, 1);

  // bar spacing 2
  label = new QLabel(tr("Bar Spacing 2"), w);
  grid->addWidget(label, 3, 0);

  bs2Spinner = new QSpinBox(2, 99, 1, w);
  bs2Spinner->setValue(ps2Button);
  connect(bs2Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs2Spinner, 3, 1);

  // bar spacing 3
  label = new QLabel(tr("Bar Spacing 3"), w);
  grid->addWidget(label, 4, 0);

  bs3Spinner = new QSpinBox(2, 99, 1, w);
  bs3Spinner->setValue(ps3Button);
  connect(bs3Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs3Spinner, 4, 1);

  addTab(w, tr("General"));
}

void Preferences::createColorPage()
{
  // colors parms page

  QWidget *w = new QWidget(this);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 3);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  grid->setColStretch(2, 2);

  vbox->insertStretch(-1, 1);

  // background color
  QLabel *label = new QLabel(tr("Chart Background"), w);
  grid->addWidget(label, 0, 0);

  backgroundColorButton = new ColorButton(w, backgroundColor);
  grid->addWidget(backgroundColorButton, 0, 1);
  backgroundColorButton->setColorButton();
  connect(backgroundColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // border color
  label = new QLabel(tr("Chart Border"), w);
  grid->addWidget(label, 1, 0);

  borderColorButton = new ColorButton(w, borderColor);
  grid->addWidget(borderColorButton, 1, 1);
  borderColorButton->setColorButton();
  connect(borderColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // grid color
  label = new QLabel(tr("Chart Grid"), w);
  grid->addWidget(label, 2, 0);

  gridColorButton = new ColorButton(w, gridColor);
  grid->addWidget(gridColorButton, 2, 1);
  gridColorButton->setColorButton();
  connect(gridColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  //FIXME: add adjustment possibility for prefered CO-colors.
  // in this way to add a spinbox to set the amount of colors too

  addTab(w, tr("Colors"));
}

void Preferences::createFontPage()
{
  // font parms page

  QWidget *w = new QWidget(this);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 3);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  grid->setColStretch(2, 2);

  vbox->insertStretch(-1, 1);

  // plot font
  QLabel *label = new QLabel(tr("Plot Font"), w);
  grid->addWidget(label, 0, 0);

  plotFontButton = new FontButton(w, plotFont);
  grid->addWidget(plotFontButton, 0, 1);
  connect(plotFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // app font
  label = new QLabel(tr("App Font"), w);
  grid->addWidget(label, 1, 0);

  appFontButton = new FontButton(w, appFont);
  grid->addWidget(appFontButton, 1, 1);
  connect(appFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  addTab(w, tr("Fonts"));

}

void  Preferences::createMTPage()
{
  // main tool bar page

  QWidget *w = new QWidget(this);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 6); // two more cols as needed
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(2, 1); // middle spacing col
  grid->setColStretch(5, 2); // outer right col

  vbox->insertStretch(-1, 1);

  int i = 0; // count rows
  int j = 0; // "count" cols
  bool tb; // temporary
  RcFile rcfile;

  QLabel *label = new QLabel(tr("Quit"), w);
  grid->addWidget(label, i, j);
  quitBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowQuitBtn, tb);
  quitBtnCheck->setChecked(tb);
  connect(quitBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(quitBtnCheck, i++, j + 1);

  label = new QLabel(tr("Preferences"), w);
  grid->addWidget(label, i, j);
  prefBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowPrefBtn, tb);
  prefBtnCheck->setChecked(tb);
  connect(prefBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(prefBtnCheck, i++, j + 1);

  label = new QLabel(tr("Side Panel"), w);
  grid->addWidget(label, i, j);
  sidePanelBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowSidePanelBtn, tb);
  sidePanelBtnCheck->setChecked(tb);
  connect(sidePanelBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(sidePanelBtnCheck, i++, j + 1);

  label = new QLabel(tr("Grid"), w);
  grid->addWidget(label, i, j);
  gridBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowGridBtn, tb);
  gridBtnCheck->setChecked(tb);
  connect(gridBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(gridBtnCheck, i++, j + 1);

  label = new QLabel(tr("Scale to Screen"), w);
  grid->addWidget(label, i, j);
  scaleToScreenBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowScaleToScreenBtn, tb);
  scaleToScreenBtnCheck->setChecked(tb);
  connect(scaleToScreenBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(scaleToScreenBtnCheck, i++, j + 1);

  label = new QLabel(tr("CrossHair"), w);
  grid->addWidget(label, i, j);
  crosshairBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCrosshairBtn, tb);
  crosshairBtnCheck->setChecked(tb);
  connect(crosshairBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(crosshairBtnCheck, i++, j + 1);

  // now fill a second col
  i = 0;
  j = 3;

  label = new QLabel(tr("PaperTrade"), w);
  grid->addWidget(label, i, j);
  paperTradeBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowPaperTradeBtn, tb);
  paperTradeBtnCheck->setChecked(tb);
  connect(paperTradeBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(paperTradeBtnCheck, i++, j + 1);

  label = new QLabel(tr("DrawMode"), w);
  grid->addWidget(label, i, j);
  drawModeBtnCheck= new QCheckBox(w);
  rcfile.loadData(RcFile::ShowDrawModeBtn, tb);
  drawModeBtnCheck->setChecked(tb);
  connect(drawModeBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(drawModeBtnCheck, i++, j + 1);

  label = new QLabel(tr("NewIndicator"), w);
  grid->addWidget(label, i, j);
  newIndicatorBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowNewIndicatorBtn, tb);
  newIndicatorBtnCheck->setChecked(tb);
  connect(newIndicatorBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(newIndicatorBtnCheck, i++, j + 1);

  label = new QLabel(tr("DataWindow"), w);
  grid->addWidget(label, i, j);
  dataWindowBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowDataWindowBtn, tb);
  dataWindowBtnCheck->setChecked(tb);
  connect(dataWindowBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(dataWindowBtnCheck, i++, j + 1);

  label = new QLabel(tr("MainQuote"), w);
  grid->addWidget(label, i, j);
  mainQuoteBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowMainQuoteBtn, tb);
  mainQuoteBtnCheck->setChecked(tb);
  connect(mainQuoteBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(mainQuoteBtnCheck, i++, j + 1);

  label = new QLabel(tr("Help"), w);
  grid->addWidget(label, i, j);
  helpButtonCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowHelpButton, tb);
  helpButtonCheck->setChecked(tb);
  connect(helpButtonCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(helpButtonCheck, i++, j + 1);

  addTab(w, tr("MainToolbar"));
}

void  Preferences::createCTPage()
{
  // chart tool bar page

  QWidget *w = new QWidget(this);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  vbox->insertStretch(-1, 1);

  int i = 0; // count rows
  int j = 0; // "count" cols
  bool tb; // temporary
  RcFile rcfile;

  QLabel *label = new QLabel(tr("Compression list"), w);
  grid->addWidget(label, i, j);
  cmpsComboBoxCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmpsComboBox, tb);
  cmpsComboBoxCheck->setChecked(tb);
  connect(cmpsComboBoxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsComboBoxCheck, i++, j + 1);

  label = new QLabel(tr("Compression 1 Minute"), w);
  grid->addWidget(label, i, j);
  cmps1MBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps1MBtn, tb);
  cmps1MBtnCheck->setChecked(tb);
  connect(cmps1MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps1MBtnCheck, i++, j + 1);

//  label = new QLabel(tr("Compression 2 Minute"), w);
//  grid->addWidget(label, i, j);
//  cmps2MBtnCheck = new QCheckBox(w);
//  rcfile.loadData(RcFile::ShowCmps2MBtn, tb);
//  cmps2MBtnCheck->setChecked(tb);
//  connect(cmps2MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
//  grid->addWidget(cmps2MBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 5 Minute"), w);
  grid->addWidget(label, i, j);
  cmps5MBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps5MBtn, tb);
  cmps5MBtnCheck->setChecked(tb);
  connect(cmps5MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps5MBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 10 Minute"), w);
  grid->addWidget(label, i, j);
  cmps10MBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps10MBtn, tb);
  cmps10MBtnCheck->setChecked(tb);
  connect(cmps10MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps10MBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 15 Minute"), w);
  grid->addWidget(label, i, j);
  cmps15MBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps15MBtn, tb);
  cmps15MBtnCheck->setChecked(tb);
  connect(cmps15MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps15MBtnCheck, i++, j + 1);

//  label = new QLabel(tr("Compression 20 Minute"), w);
//  grid->addWidget(label, i, j);
//  cmps20MBtnCheck = new QCheckBox(w);
//  rcfile.loadData(RcFile::ShowCmps20MBtn, tb);
//  cmps20MBtnCheck->setChecked(tb);
//  connect(cmps20MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
//  grid->addWidget(cmps20MBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 30 Minute"), w);
  grid->addWidget(label, i, j);
  cmps30MBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps30MBtn, tb);
  cmps30MBtnCheck->setChecked(tb);
  connect(cmps30MBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps30MBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 1 Hour"), w);
  grid->addWidget(label, i, j);
  cmps1HBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps1HBtn, tb);
  cmps1HBtnCheck->setChecked(tb);
  connect(cmps1HBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps1HBtnCheck, i++, j + 1);

//  label = new QLabel(tr("Compression 2 Hour"), w);
//  grid->addWidget(label, i, j);
//  cmps2HBtnCheck = new QCheckBox(w);
//  rcfile.loadData(RcFile::ShowCmps2HBtn, tb);
//  cmps2HBtnCheck->setChecked(tb);
//  connect(cmps2HBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
//  grid->addWidget(cmps2HBtnCheck, i++, j + 1);

//  label = new QLabel(tr("Compression 4 Hour"), w);
//  grid->addWidget(label, i, j);
//  cmps4HBtnCheck = new QCheckBox(w);
//  rcfile.loadData(RcFile::ShowCmps4HBtn, tb);
//  cmps4HBtnCheck->setChecked(tb);
//  connect(cmps4HBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
//  grid->addWidget(cmps4HBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 1 Day"), w);
  grid->addWidget(label, i, j);
  cmps1DBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps1DBtn, tb);
  cmps1DBtnCheck->setChecked(tb);
  connect(cmps1DBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps1DBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 1 Week"), w);
  grid->addWidget(label, i, j);
  cmps1WBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmps1WBtn, tb);
  cmps1WBtnCheck->setChecked(tb);
  connect(cmps1WBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps1WBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 1 Month"), w);
  grid->addWidget(label, i, j);
  cmpsMNBtnCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowCmpsMNBtn, tb);
  cmpsMNBtnCheck->setChecked(tb);
  connect(cmpsMNBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsMNBtnCheck, i++, j + 1);

  label = new QLabel(tr("BarSpacing spinner"), w);
  grid->addWidget(label, i, j);
  barSpSpinboxCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowBarSpSpinbox, tb);
  barSpSpinboxCheck->setChecked(tb);
  connect(barSpSpinboxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(barSpSpinboxCheck, i++, j + 1);

  label = new QLabel(tr("BarsToLoad field"), w);
  grid->addWidget(label, i, j);
  barsToLoadFieldCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowBarsToLoadField, tb);
  barsToLoadFieldCheck->setChecked(tb);
  connect(barsToLoadFieldCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(barsToLoadFieldCheck, i++, j + 1);

  label = new QLabel(tr("Pan Chart slider"), w);
  grid->addWidget(label, i, j);
  sliderCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowSlider, tb);
  sliderCheck->setChecked(tb);
  connect(sliderCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(sliderCheck, i++, j + 1);

  addTab(w, tr("ChartToolbar"));
}

void  Preferences::createETPage()
{
  // extra tool bar page

  QWidget *w = new QWidget(this);
  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  vbox->insertStretch(-1, 1);

  int i = 0; // count rows
  int j = 0; // "count" cols
  bool tb; // temporary
  RcFile rcfile;

  QLabel *label = new QLabel(tr("Recent charts"), w);
  grid->addWidget(label, i, j);
  recentComboBoxCheck = new QCheckBox(w);
  rcfile.loadData(RcFile::ShowRecentCharts, tb);
  recentComboBoxCheck->setChecked(tb);
  connect(recentComboBoxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(recentComboBoxCheck, i++, j + 1);

  addTab(w, tr("ExtraToolbar"));
}

void Preferences::slotSave()
{
  RcFile rcfile;

  bool tbool = menubarCheck->isChecked();
  if (tbool != menubar)
  {
    rcfile.saveData(RcFile::ShowMenuBar,tbool);
    emit signalMenubar(tbool);
    menubar = tbool;
  }

  tbool = extraToolbarCheck->isChecked();
  if (tbool != extraToolbar)
  {
    rcfile.saveData(RcFile::ShowExtraToolbar,tbool);
    emit signalExtraToolbar(tbool);
    extraToolbar = tbool;
   }

  int tint = bs1Spinner->value();
  if (tint != ps1Button)
  {
    rcfile.saveData(RcFile::PSButton, tint, 1);
    ps1Button = tint;
  }

  tint = bs2Spinner->value();
  if (tint != ps2Button)
  {
    rcfile.saveData(RcFile::PSButton, tint, 2);
    ps2Button = tint;
  }

  tint = bs3Spinner->value();
  if (tint != ps3Button)
  {
    rcfile.saveData(RcFile::PSButton, tint, 3);
    ps3Button = tint;
  }

  bool flag = FALSE;
  QColor c;
  backgroundColorButton->getColor(c);
  if (c != backgroundColor)
  {
    rcfile.saveColor(RcFile::BackgroundColor,c);
    emit signalBackgroundColor(c);
    flag = TRUE;
    backgroundColor = c;
  }

  borderColorButton->getColor(c);
  if (c != borderColor)
  {
    rcfile.saveColor(RcFile::BorderColor,c);
    emit signalBorderColor(c);
    flag = TRUE;
    borderColor = c;
  }

  gridColorButton->getColor(c);
  if (c != gridColor)
  {
    rcfile.saveColor(RcFile::GridColor,c);
    emit signalGridColor(c);
    flag = TRUE;
    gridColor = c;
  }

  QFont f;
  plotFontButton->getFont(f);
  if (f != plotFont)
  {
    rcfile.saveFont(RcFile::PlotFont,f);
    emit signalPlotFont(f);
    flag = TRUE;
    plotFont = f;
  }

  appFontButton->getFont(f);
  if (f != appFont)
  {
    rcfile.saveFont(RcFile::AppFont,f);
    emit signalAppFont(f);
    flag = TRUE;
    appFont = f;
  }

  // main tool bar settings
  // save all, anyway if changed or not, who cares?
  rcfile.saveData(RcFile::ShowQuitBtn, quitBtnCheck->isChecked());

  // prevent the user does things he will regret...
  if(menubarCheck->isChecked())
      rcfile.saveData(RcFile::ShowPrefBtn, prefBtnCheck->isChecked());
  else
      rcfile.saveData(RcFile::ShowPrefBtn, TRUE);

  rcfile.saveData(RcFile::ShowSidePanelBtn, sidePanelBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowGridBtn, gridBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowScaleToScreenBtn, scaleToScreenBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCrosshairBtn, crosshairBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowPaperTradeBtn, paperTradeBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowDrawModeBtn, drawModeBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowNewIndicatorBtn, newIndicatorBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowDataWindowBtn, dataWindowBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowMainQuoteBtn, mainQuoteBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowHelpButton, helpButtonCheck->isChecked());
  // chart tool bar settings
  rcfile.saveData(RcFile::ShowSlider, sliderCheck->isChecked());
  rcfile.saveData(RcFile::ShowBarsToLoadField, barsToLoadFieldCheck->isChecked());
  rcfile.saveData(RcFile::ShowBarSpSpinbox, barSpSpinboxCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps1MBtn, cmps1MBtnCheck->isChecked());
//  rcfile.saveData(RcFile::ShowCmps2MBtn, cmps2MBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps5MBtn, cmps5MBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps10MBtn, cmps10MBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps15MBtn, cmps15MBtnCheck->isChecked());
//  rcfile.saveData(RcFile::ShowCmps20MBtn, cmps20MBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps30MBtn, cmps30MBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps1HBtn, cmps1HBtnCheck->isChecked());
//  rcfile.saveData(RcFile::ShowCmps2HBtn, cmps2HBtnCheck->isChecked());
//  rcfile.saveData(RcFile::ShowCmps4HBtn, cmps4HBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps1DBtn, cmps1DBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmps1WBtn, cmps1WBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmpsMNBtn, cmpsMNBtnCheck->isChecked());
  rcfile.saveData(RcFile::ShowCmpsComboBox, cmpsComboBoxCheck->isChecked());

  rcfile.saveData(RcFile::ShowRecentCharts, recentComboBoxCheck->isChecked());

  emit signalReloadToolBars();

  if (flag)
    emit signalLoadChart();

  setCancelButton(tr("&OK"));
}

void Preferences::slotHelp()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}


void Preferences::slotModified()
{
  // FIXME: The goal is to change the caption of the
  // Cancel-button "Cancel/OK" 	depending on a change
  // of any data by the user
  //qDebug("changed");
  setCancelButton(tr("&Cancel"));
}
