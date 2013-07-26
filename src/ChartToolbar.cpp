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

#include "ChartToolbar.h"
//#include "Config.h"
//#include "RcFile.h"
#include "BarData.h"
#include "PrefDialog.h"
#include <qtooltip.h>
#include <q3accel.h>
#include <qvalidator.h>
//Added by qt3to4:
#include <QKeyEvent>
#include "../pics/date.xpm"
#include "../pics/next.xpm"

ChartToolbar::ChartToolbar(QMainWindow *mw) : QToolBar("chartToolbar", mw )
{
  minPixelspace = 2;

  QString ts; // temporary string
  int ti;  // temporary integer

  ptDate = QDateTime::currentDateTime();

  BarData bd(ts);
  compressionCombo = new QComboBox(this);
  compressionCAction = addWidget(compressionCombo);
  bd.getBarLengthList(compressionList);
  compressionCombo->insertStringList(compressionList, -1);
  rcfile.loadData(RcFile::BarLength, ti);
  compressionCombo->setCurrentItem((BarData::BarLength) ti);
  QToolTip::add(compressionCombo, tr("Bar Length (Compression)"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SLOT(compressionChanged(int)));
  connect(compressionCombo, SIGNAL(activated(int)), this, SIGNAL(signalBarLengthChanged(int)));

  cmpsBtn1M = new QToolButton(this);
  cmpsAct1M = addWidget(cmpsBtn1M);
  QToolTip::add(cmpsBtn1M, tr("1 Minute Compression"));
  cmpsBtn1M->setToggleButton(TRUE);
  connect(cmpsBtn1M, SIGNAL(clicked()), this, SLOT(cmpsBtn1MClicked()));

//  cmpsBtn2M = new QToolButton(this);
//  cmpsAct2M = addWidget(cmpsBtn2M);
//  QToolTip::add(cmpsBtn2M, tr("2 Minute Compression"));
//  cmpsBtn2M->setToggleButton(TRUE);
//  connect(cmpsBtn2M, SIGNAL(clicked()), this, SLOT(cmpsBtn2MClicked()));

  cmpsBtn5M = new QToolButton(this);
  cmpsAct5M = addWidget(cmpsBtn5M);
  QToolTip::add(cmpsBtn5M, tr("5 Minute Compression"));
  cmpsBtn5M->setToggleButton(TRUE);
  connect(cmpsBtn5M, SIGNAL(clicked()), this, SLOT(cmpsBtn5MClicked()));

  cmpsBtn10M = new QToolButton(this);
  cmpsAct10M = addWidget(cmpsBtn10M);
  QToolTip::add(cmpsBtn10M, tr("10 Minute Compression"));
  cmpsBtn10M->setToggleButton(TRUE);
  connect(cmpsBtn10M, SIGNAL(clicked()), this, SLOT(cmpsBtn10MClicked()));

  cmpsBtn15M = new QToolButton(this);
  cmpsAct15M = addWidget(cmpsBtn15M);
  QToolTip::add(cmpsBtn15M, tr("15 Minute Compression"));
  cmpsBtn15M->setToggleButton(TRUE);
  connect(cmpsBtn15M, SIGNAL(clicked()), this, SLOT(cmpsBtn15MClicked()));

//  cmpsBtn20M = new QToolButton(this);
//  cmpsAct20M = addWidget(cmpsBtn20M);
//  QToolTip::add(cmpsBtn20M, tr("20 Minute Compression"));
//  cmpsBtn20M->setToggleButton(TRUE);
//  connect(cmpsBtn20M, SIGNAL(clicked()), this, SLOT(cmpsBtn20MClicked()));

  cmpsBtn30M = new QToolButton(this);
  cmpsAct30M = addWidget(cmpsBtn30M);
  QToolTip::add(cmpsBtn30M, tr("30 Minute Compression"));
  cmpsBtn30M->setToggleButton(TRUE);
  connect(cmpsBtn30M, SIGNAL(clicked()), this, SLOT(cmpsBtn30MClicked()));

  cmpsBtn1H = new QToolButton(this);
  cmpsAct1H = addWidget(cmpsBtn1H);
  QToolTip::add(cmpsBtn1H, tr("1 Hour Compression"));
  cmpsBtn1H->setToggleButton(TRUE);
  connect(cmpsBtn1H, SIGNAL(clicked()), this, SLOT(cmpsBtn1HClicked()));

//  cmpsBtn2H = new QToolButton(this);
//  cmpsAct2H = addWidget(cmpsBtn2H);
//  QToolTip::add(cmpsBtn2H, tr("2 Hour Compression"));
//  cmpsBtn2H->setToggleButton(TRUE);
//  connect(cmpsBtn2H, SIGNAL(clicked()), this, SLOT(cmpsBtn2HClicked()));

//  cmpsBtn4H = new QToolButton(this);
//  cmpsAct4H = addWidget(cmpsBtn4H);
//  QToolTip::add(cmpsBtn4H, tr("4 Hour Compression"));
//  cmpsBtn4H->setToggleButton(TRUE);
//  connect(cmpsBtn4H, SIGNAL(clicked()), this, SLOT(cmpsBtn4HClicked()));

  cmpsBtn1D = new QToolButton(this);
  cmpsAct1D = addWidget(cmpsBtn1D);
  QToolTip::add(cmpsBtn1D, tr("1 Day Compression"));
  cmpsBtn1D->setToggleButton(TRUE);
  connect(cmpsBtn1D, SIGNAL(clicked()), this, SLOT(cmpsBtn1DClicked()));

  cmpsBtn1W = new QToolButton(this);
  cmpsAct1W = addWidget(cmpsBtn1W);
  QToolTip::add(cmpsBtn1W, tr("1 Week Compression"));
  cmpsBtn1W->setToggleButton(TRUE);
  connect(cmpsBtn1W, SIGNAL(clicked()), this, SLOT(cmpsBtn1WClicked()));

  cmpsBtnMN = new QToolButton(this);
  cmpsActMN = addWidget(cmpsBtnMN);
  QToolTip::add(cmpsBtnMN, tr("1 Month Compression"));
  cmpsBtnMN->setToggleButton(TRUE);
  connect(cmpsBtnMN, SIGNAL(clicked()), this, SLOT(cmpsBtnMNClicked()));

  compressionChanged((BarData::BarLength) ti);

  addSeparator();

  ps3Button = new QToolButton(this);
  ps3Button->setToggleButton(TRUE);
  rcfile.loadData(RcFile::PSButton, ps3ButtonText, 3);

  ps2Button = new QToolButton(this);
  ps2Button->setToggleButton(TRUE);
  rcfile.loadData(RcFile::PSButton, ps2ButtonText, 2);
  if (ps2ButtonText == ps3ButtonText && ps3ButtonText == 6)
  {
    ps2ButtonText = 4;
    rcfile.saveData(RcFile::PSButton, ps2ButtonText, 2);
  }

  ps1Button = new QToolButton(this);
  ps1Button->setToggleButton(TRUE);
  rcfile.loadData(RcFile::PSButton, ps1ButtonText, 1);
  if (ps1ButtonText == ps3ButtonText && ps3ButtonText == 6)
  {
    ps1ButtonText = 2;
    rcfile.saveData(RcFile::PSButton, ps1ButtonText, 1);
  }

  pixelspace = new QSpinBox(this);
  pixelspaceAction = addWidget(pixelspace);
  pixelspace->setRange(minPixelspace, 9);
  rcfile.loadData(RcFile::Pixelspace, ti);
  connect(pixelspace, SIGNAL(valueChanged(int)), this, SIGNAL(signalPixelspaceChanged(int)));
  connect(pixelspace, SIGNAL(valueChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  pixelspace->setValue(ti);
  QToolTip::add(pixelspace, tr("Bar Spacing"));

  addWidget(ps1Button);
  connect(ps1Button, SIGNAL(clicked()), this, SLOT(ps1ButtonClicked()));

  addWidget(ps2Button);
  connect(ps2Button, SIGNAL(clicked()), this, SLOT(ps2ButtonClicked()));

  addWidget(ps3Button);
  connect(ps3Button, SIGNAL(clicked()), this, SLOT(ps3ButtonClicked()));

  addSeparator();

  barCount = new QLineEdit(this);
  barCountAction = addWidget(barCount);
  // regexp: a non-zero digit followed by 0 to 4 other digits
  QRegExp rx("[1-9]\\d{0,4}");
  QValidator *rv = new QRegExpValidator(rx, this);
  barCount->setValidator(rv);
  rcfile.loadData(RcFile::BarsToLoad, ti);
  barCount->setText(QString::number(ti));
  QToolTip::add(barCount, tr("Total bars to load"));
  connect(barCount, SIGNAL(lostFocus()), this, SLOT(barsChangedValidate()));
  connect(barCount, SIGNAL(returnPressed()), this, SLOT(barsChanged()));

  addSeparator();

  ptdButton = new QToolButton(this);
  ptdAction = addWidget(ptdButton);
  QToolTip::add(ptdButton, tr("Paper Trade Date"));
  ptdButton->setPixmap(date);
  connect(ptdButton, SIGNAL(clicked()), this, SLOT(paperTradeDate()));

  ptnButton = new QToolButton(this);
  ptnAction = addWidget(ptnButton);
  QToolTip::add(ptnButton, tr("Load Next Bar"));
  ptnButton->setPixmap(next);
  connect(ptnButton, SIGNAL(clicked()), this, SLOT(paperTradeNextBar()));
  //ptnButton->setFocusPolicy(QWidget::StrongFocus);

  slider = new QSlider(this);
  sliderAction = addWidget(slider);
  slider->setOrientation(Qt::Horizontal);
  connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalSliderChanged(int)));
  slider->setEnabled(FALSE);
  QToolTip::add(slider, tr("Pan Chart"));

  slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  //setStretchableWidget(slider);

  Q3Accel *a = new Q3Accel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_End, ChartPannerFocus);
  a->insertItem(Qt::CTRL+Qt::Key_Plus, BarsLoadedFocus);
  a->insertItem(Qt::CTRL+Qt::Key_Minus, BarSpacingFocus);
  a->insertItem(Qt::CTRL+Qt::Key_Prior, BarLengthFocus);
  a->insertItem(Qt::CTRL+Qt::Key_B, ToolbarFocus);

  focusFlag = BarLengthFocus;

  connect(this, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(slotOrientationChanged(Qt::Orientation)));

  //setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  //setVerticallyStretchable(TRUE);
}

ChartToolbar::~ChartToolbar()
{
}

int ChartToolbar::getBars()
{
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (ok)
    return t;
  else
    return 0;
}

void ChartToolbar::enableSlider(bool d)
{
  slider->setEnabled(d);
}

void ChartToolbar::setPixelspace(int min, int d)
{
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(min, 9);
  pixelspace->setValue(d);
  pixelspace->blockSignals(FALSE);
}

int ChartToolbar::getPixelspace()
{
  return pixelspace->value();
}

int ChartToolbar::getBarLengthInt()
{
  return compressionCombo->currentItem();
}

QString ChartToolbar::getBarLength()
{
  return compressionCombo->currentText();
}

int ChartToolbar::getSlider()
{
  return slider->value();
}

int ChartToolbar::setSliderStart(int width, int records)
{
  int page = width / getPixelspace();
  int max = records - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, records - 1);
  int rc = 0;

  if ((int) records < page)
    slider->setValue(0);
  else
  {
    slider->setValue(max + 1);
    rc = max + 1;
  }

  slider->blockSignals(FALSE);

  return rc;
}

void ChartToolbar::saveSettings()
{
  rcfile.saveData(RcFile::BarsToLoad, getBars());
  rcfile.saveData(RcFile::BarLength, getBarLengthInt());
  rcfile.saveData(RcFile::Pixelspace, getPixelspace());
  //rcfile.savePoint(RcFile::ChartToolBarPos, pos());
  //FIXME: IMPORTANT- save all belonged to restore the position of the toolbar. maybe not here but in Qtstalker.cpp. Possible is this the way
  //http://doc.trolltech.com/3.3/qdockarea.html#operator-lt-lt
}

void ChartToolbar::slotSetButtonView()
{
  // do all relating to visual aspekts

  QString ts;    // temporary variables
  int ti;
  bool tb;

  rcfile.loadData(RcFile::ShowCmpsComboBox, tb);
  if (tb) compressionCAction->setVisible(true);
  else compressionCAction->setVisible(false);

  rcfile.loadData(RcFile::ShowCmps1MBtn, tb);
  if (tb) cmpsAct1M->setVisible(true);
  else cmpsAct1M->setVisible(false);
  cmpsBtn1M->setText("1M");

//  rcfile.loadData(RcFile::ShowCmps2MBtn, tb);
//  if (tb) cmpsAct2M->setVisible(true);
//  else cmpsAct2M->setVisible(false);
//  cmpsBtn2M->setText("2M");

  rcfile.loadData(RcFile::ShowCmps5MBtn, tb);
  if (tb) cmpsAct5M->setVisible(true);
  else cmpsAct5M->setVisible(false);
  cmpsBtn5M->setText("5M");

  rcfile.loadData(RcFile::ShowCmps10MBtn, tb);
  if (tb) cmpsAct10M->setVisible(true);
  else cmpsAct10M->setVisible(false);
  cmpsBtn10M->setText("10M");

  rcfile.loadData(RcFile::ShowCmps15MBtn, tb);
  if (tb) cmpsAct15M->setVisible(true);
  else cmpsAct15M->setVisible(false);
  cmpsBtn15M->setText("15M");

//  rcfile.loadData(RcFile::ShowCmps20MBtn, tb);
//  if (tb) cmpsAct20M->setVisible(true);
//  else cmpsAct20M->setVisible(false);
//  cmpsBtn20M->setText("20M");

  rcfile.loadData(RcFile::ShowCmps30MBtn, tb);
  if (tb) cmpsAct30M->setVisible(true);
  else cmpsAct30M->setVisible(false);
  cmpsBtn30M->setText("30M");

  rcfile.loadData(RcFile::ShowCmps1HBtn, tb);
  if (tb) cmpsAct1H->setVisible(true);
  else cmpsAct1H->setVisible(false);
  cmpsBtn1H->setText("1H");

//  rcfile.loadData(RcFile::ShowCmps2HBtn, tb);
//  if (tb) cmpsAct2H->setVisible(true);
//  else cmpsAct2H->setVisible(false);
//  cmpsBtn2H->setText("2H");

//  rcfile.loadData(RcFile::ShowCmps4HBtn, tb);
//  if (tb) cmpsAct4H->setVisible(true);
//  else cmpsAct4H->setVisible(false);
//  cmpsBtn4H->setText("4H");

  rcfile.loadData(RcFile::ShowCmps1DBtn, tb);
  if (tb) cmpsAct1D->setVisible(true);
  else cmpsAct1D->setVisible(false);
  cmpsBtn1D->setText("1D");

  rcfile.loadData(RcFile::ShowCmps1WBtn, tb);
  if (tb) cmpsAct1W->setVisible(true);
  else cmpsAct1W->setVisible(false);
  cmpsBtn1W->setText("1W");

  rcfile.loadData(RcFile::ShowCmpsMNBtn, tb);
  if (tb) cmpsActMN->setVisible(true);
  else cmpsActMN->setVisible(false);
  cmpsBtnMN->setText("MN");

  rcfile.loadData(RcFile::ShowBarSpSpinbox, tb);
  if (tb) pixelspaceAction->setVisible(true);
  else pixelspaceAction->setVisible(false);
  pixelspace->setMaximumWidth(40); // FIXME:calc as a function of app font metrics

  rcfile.loadData(RcFile::PSButton, ps1ButtonText, 1);
  ts = QString::number(ps1ButtonText);
  QToolTip::add(ps1Button, tr("Set Bar Spacing to " + ts));
  ps1Button->setText(ts);

  ts = QString::number(ps2ButtonText);
  QToolTip::add(ps2Button, tr("Set Bar Spacing to " + ts));
  ps2Button->setText(ts);

  ts = QString::number(ps3ButtonText);
  QToolTip::add(ps3Button, tr("Set Bar Spacing to " + ts));
  ps3Button->setText(ts);

  rcfile.loadData(RcFile::ShowBarsToLoadField, tb);
  if (tb) barCountAction->setVisible(true);
  else barCountAction->setVisible(false);
  barCount->setMaximumWidth(40);// FIXME:calc as a function of app font metrics

  rcfile.loadData(RcFile::ShowSlider, tb);
  if (tb) sliderAction->setVisible(true);
  else sliderAction->setVisible(false);

}
void ChartToolbar::setFocus()
{
  compressionCombo->setFocus();
  focusFlag = BarLengthFocus;
}

void ChartToolbar::slotAccel(int id)
{
  switch(id)
  {
    case BarLengthFocus:
      compressionCombo->setFocus();
      focusFlag = BarLengthFocus;
      break;
    case ChartPannerFocus:
      slider->setFocus();
      focusFlag = ChartPannerFocus;
      break;
    case BarsLoadedFocus:
      barCount->setFocus();
      focusFlag = BarsLoadedFocus;
      break;
    case BarSpacingFocus:
      pixelspace->setFocus();
      focusFlag = BarSpacingFocus;
      break;
    case ToolbarFocus:
      setFocus();
      break;
    default:
      break;
  }
}

void ChartToolbar::doKeyPress(QKeyEvent *key)
{
  key->accept();

  if (! key->state())
  {
    switch(focusFlag)
    {
      case BarLengthFocus:
//        compressionCombo->doKeyPress(key);
	break;
      case BarSpacingFocus:
//        pixelspace->doKeyPress(key);
	break;
      case BarsLoadedFocus:
//        barCount->doKeyPress(key);
	break;
      case ChartPannerFocus:
//        slider->doKeyPress(key);
	break;
      default:
        break;
    }
  }
  else
  {
    if (key->state() == Qt::ControlButton)
    {
      switch(key->key())
      {
        case Qt::Key_Prior:
	  slotAccel(BarLengthFocus);
          break;
        case Qt::Key_Plus:
	  slotAccel(BarsLoadedFocus);
          break;
        case Qt::Key_End:
	  slotAccel(ChartPannerFocus);
          break;
        case Qt::Key_Minus:
	  slotAccel(BarSpacingFocus);
          break;
        case Qt::Key_B:
	  slotAccel(ToolbarFocus);
          break;
        default:
          break;
      }
    }
  }
}

void ChartToolbar::barsChanged()
{
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (ok)
    emit signalBarsChanged(t);
}

void ChartToolbar::barsChangedValidate()
{
  // The barCount validator ensures non-zero integer value, but if users moves
  // focus without completing the text entry, then app can try to load zero bars
  // which would cause grief.
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (!ok || t == 0)
    barCount->setText("250");
}

void ChartToolbar::ps1ButtonClicked()
{
  int ti;
  rcfile.loadData(RcFile::PSButton, ti, 1);
  pixelspace->setValue(ti);
  emit slotPixelspaceChanged(ti);
}

void ChartToolbar::ps2ButtonClicked()
{
  int ti;
  rcfile.loadData(RcFile::PSButton, ti, 2);
  pixelspace->setValue(ti);
  emit slotPixelspaceChanged(ti);
}

void ChartToolbar::ps3ButtonClicked()
{
  int ti;
  rcfile.loadData(RcFile::PSButton, ti, 3);
  pixelspace->setValue(ti);
  emit slotPixelspaceChanged(ti);
}

void ChartToolbar::cmpsBtn1MClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)0);
  compressionChanged(0);
  emit signalBarLengthChanged((int)0);
}

//void ChartToolbar::cmpsBtn2MClicked()
//{
//}

void ChartToolbar::cmpsBtn5MClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)1);
  compressionChanged(1);
  emit signalBarLengthChanged((int)1);
}

void ChartToolbar::cmpsBtn10MClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)2);
  compressionChanged(2);
  emit signalBarLengthChanged((int)2);
}

void ChartToolbar::cmpsBtn15MClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)3);
  compressionChanged(3);
  emit signalBarLengthChanged((int)3);
}

//void ChartToolbar::cmpsBtn20MClicked()
//{
//}

void ChartToolbar::cmpsBtn30MClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)4);
  compressionChanged(4);
  emit signalBarLengthChanged((int)4);
}

void ChartToolbar::cmpsBtn1HClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)5);
  compressionChanged(5);
  emit signalBarLengthChanged((int)5);
}

//void ChartToolbar::cmpsBtn2HClicked()
//{
//}

//void ChartToolbar::cmpsBtn4HClicked()
//{
//}

void ChartToolbar::cmpsBtn1DClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)6);
  compressionChanged(6);
  emit signalBarLengthChanged((int)6);
}

void ChartToolbar::cmpsBtn1WClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)7);
  compressionChanged(7);
  emit signalBarLengthChanged((int)7);
}

void ChartToolbar::cmpsBtnMNClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)8);
  compressionChanged(8);
  emit signalBarLengthChanged((int)8);
}

void ChartToolbar::compressionChanged(int compression)
{
  compression == 0 ? cmpsBtn1M->setOn(TRUE) : cmpsBtn1M->setOn(FALSE);
//  compression == 1 ? cmpsBtn2M->setOn(TRUE) : cmpsBtn2M->setOn(FALSE);
  compression == 1 ? cmpsBtn5M->setOn(TRUE) : cmpsBtn5M->setOn(FALSE);
  compression == 2 ? cmpsBtn10M->setOn(TRUE) : cmpsBtn10M->setOn(FALSE);
  compression == 3 ? cmpsBtn15M->setOn(TRUE) : cmpsBtn15M->setOn(FALSE);
//  compression == 4 ? cmpsBtn20M->setOn(TRUE) : cmpsBtn20M->setOn(FALSE);
  compression == 4 ? cmpsBtn30M->setOn(TRUE) : cmpsBtn30M->setOn(FALSE);
  compression == 5 ? cmpsBtn1H->setOn(TRUE) : cmpsBtn1H->setOn(FALSE);
//  compression == 5 ? cmpsBtn2H->setOn(TRUE) : cmpsBtn2H->setOn(FALSE);
//  compression == 5 ? cmpsBtn4H->setOn(TRUE) : cmpsBtn4H->setOn(FALSE);
  compression == 6 ? cmpsBtn1D->setOn(TRUE) : cmpsBtn1D->setOn(FALSE);
  compression == 7 ? cmpsBtn1W->setOn(TRUE) : cmpsBtn1W->setOn(FALSE);
  compression == 8 ? cmpsBtnMN->setOn(TRUE) : cmpsBtnMN->setOn(FALSE);
}

void ChartToolbar::slotOrientationChanged(Qt::Orientation o)
{
    slider->setOrientation(o);
   // updateGeometry();

 // if(o)
 // {

 // }
}

void ChartToolbar::slotPixelspaceChanged(int ps)
{
  ps1Button->setOn(FALSE);
  ps2Button->setOn(FALSE);
  ps3Button->setOn(FALSE);

  if (ps == ps1ButtonText)
    ps1Button->setOn(TRUE);
  if (ps == ps2ButtonText)
    ps2Button->setOn(TRUE);
  if (ps == ps3ButtonText)
    ps3Button->setOn(TRUE);
}

//*********************************************************************
//******************** paper trade stuff ******************************
//*********************************************************************

void ChartToolbar::getPaperTradeDate(QDateTime &d)
{
  d = ptDate;
}

void ChartToolbar::paperTradeDate()
{
  QString dl = tr("Last Date");
  QString tl = tr("Last Time");
  QString pl = "Date";

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Paper trade date select."));
  dialog->createPage(pl);
  dialog->addDateItem(dl, pl, ptDate);
  dialog->addTimeItem(tl, pl, ptDate);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QDateTime dt;
    dialog->getDate(dl, dt);
    ptDate.setDate(dt.date());

    dialog->getDate(tl, dt);
    ptDate.setTime(dt.time());

    emit signalPaperTradeNextBar();
  }

  delete dialog;
}

void ChartToolbar::paperTradeClicked(bool d)
{
  if (d)
  {
    ptdAction->setVisible(true);
    ptnAction->setVisible(true);
    barCount->setEnabled(FALSE);
  }
  else
  {
    ptdAction->setVisible(false);
    ptnAction->setVisible(false);
    barCount->setEnabled(TRUE);
  }
}

void ChartToolbar::paperTradeNextBar()
{
  if (!ptdAction->isVisible())
    return;

  switch (compressionCombo->currentItem())
  {
    case 0:
      ptDate = ptDate.addSecs(60);
      break;
    case 1:
      ptDate = ptDate.addSecs(300);
      break;
    case 2:
      ptDate = ptDate.addSecs(600);
      break;
    case 3:
      ptDate = ptDate.addSecs(900);
      break;
    case 4:
      ptDate = ptDate.addSecs(1800);
      break;
    case 5:
      ptDate = ptDate.addSecs(3600);
      break;
    case 6: // daily
      ptDate = ptDate.addDays(1);
      if (ptDate.date().dayOfWeek() == 6)
        ptDate = ptDate.addDays(2);
      else
      {
        if (ptDate.date().dayOfWeek() == 7)
          ptDate = ptDate.addDays(1);
      }
      break;
    case 7: // weekly
      ptDate = ptDate.addDays(7);
      break;
    case 8: // monthly
      ptDate = ptDate.addMonths(1);
      break;
    default:
      break;
  }

  emit signalPaperTradeNextBar();
}
