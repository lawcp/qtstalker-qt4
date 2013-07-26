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

#ifndef CHARTTOOLBAR_HPP
#define CHARTTOOLBAR_HPP

#include <qstring.h>
#include <qtoolbar.h>
#include <qmainwindow.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qtoolbutton.h>
#include <qdatetime.h>
#include <qaction.h>
//Added by qt3to4:
#include <QKeyEvent>

#include "RcFile.h"

class ChartToolbar : public QToolBar
{
  Q_OBJECT

  signals:
    void signalBarLengthChanged(int);
    void signalPixelspaceChanged(int);
    void signalSliderChanged(int);
    void signalBarsChanged(int);
    void signalPaperTradeNextBar();

  public:

    enum MenuAction
    {
      ToolbarFocus,
      BarLengthFocus,
      BarSpacingFocus,
      BarsLoadedFocus,
      ChartPannerFocus
    };

    ChartToolbar(QMainWindow *);
    ~ChartToolbar();
    int getBars();
    void enableSlider(bool);
    void setPixelspace(int, int);
    int getPixelspace();
    int getBarLengthInt();
    QString getBarLength();
    int getSlider();
    int setSliderStart(int width, int records);
    void saveSettings();
    void getPaperTradeDate(QDateTime &);


  public slots:
    void setFocus();
    void slotAccel(int);
    void doKeyPress(QKeyEvent *);
    void barsChanged();
    void paperTradeDate();
    void paperTradeNextBar();
    void paperTradeClicked(bool);
    void ps1ButtonClicked();
    void ps2ButtonClicked();
    void ps3ButtonClicked();
    void cmpsBtn1MClicked();
//    void cmpsBtn2MClicked();
    void cmpsBtn5MClicked();
    void cmpsBtn10MClicked();
    void cmpsBtn15MClicked();
//    void cmpsBtn20MClicked();
    void cmpsBtn30MClicked();
    void cmpsBtn1HClicked();
//    void cmpsBtn2HClicked();
//    void cmpsBtn4HClicked();
    void cmpsBtn1DClicked();
    void cmpsBtn1WClicked();
    void cmpsBtnMNClicked();
    void slotSetButtonView();
    void compressionChanged(int);

  private:
    QComboBox *compressionCombo;
    QAction *compressionCAction;
    QSpinBox *pixelspace;
    QAction *pixelspaceAction;
    QLineEdit *barCount;
    QAction *barCountAction;
    QSlider *slider;
    QAction *sliderAction;
    MenuAction focusFlag;
    QToolButton *ptdButton;
    QAction *ptdAction;
    QToolButton *ptnButton;
    QAction *ptnAction;
    QDateTime ptDate;
    QStringList compressionList;
    QAction *cmpsAct1M;
    QToolButton *cmpsBtn1M;
//    QAction *cmpsAct2M;
//    QToolButton *cmpsBtn2M;
    QAction *cmpsAct5M;
    QToolButton *cmpsBtn5M;
    QAction *cmpsAct10M;
    QToolButton *cmpsBtn10M;
    QAction *cmpsAct15M;
    QToolButton *cmpsBtn15M;
//    QAction *cmpsAct20M;
//    QToolButton *cmpsBtn20M;
    QAction *cmpsAct30M;
    QToolButton *cmpsBtn30M;
    QAction *cmpsAct1H;
    QToolButton *cmpsBtn1H;
//    QAction *cmpsAct2H;
//    QToolButton *cmpsBtn2H;
//    QAction *cmpsAct4H;
//    QToolButton *cmpsBtn4H;
    QAction *cmpsAct1D;
    QToolButton *cmpsBtn1D;
    QAction *cmpsAct1W;
    QToolButton *cmpsBtn1W;
    QAction *cmpsActMN;
    QToolButton *cmpsBtnMN;
    QToolButton *ps1Button;
    QToolButton *ps2Button;
    QToolButton *ps3Button;

    int minPixelspace;
    int ps1ButtonText;
    int ps2ButtonText;
    int ps3ButtonText;
    RcFile rcfile;

  private slots:
    void slotOrientationChanged(Qt::Orientation);
    void slotPixelspaceChanged(int);
    void barsChangedValidate();
};

#endif
