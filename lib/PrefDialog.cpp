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

#include "PrefDialog.h"
#include "HelpWindow.h"
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <QLabel>
#include <Q3VBoxLayout>

PrefDialog::PrefDialog (QWidget *w) : Q3TabDialog (w, "PrefDialog", TRUE)
{
  init();
}

PrefDialog::PrefDialog () : Q3TabDialog (0, "PrefDialog", TRUE)
{
  init();
}

PrefDialog::~PrefDialog ()
{
  widgetList.clear();
  gridList.clear();
  colorButtonList.clear();
  intList.clear();
  doubleList.clear();
  checkList.clear();
  fontButtonList.clear();
  textList.clear();
  comboList.clear();
  dateList.clear();
  fileList.clear();
  symbolList.clear();
  dvList.clear();
  labelList.clear();
  timeList.clear();
}

void PrefDialog::init ()
{
  widgetList.setAutoDelete(FALSE);
  gridList.setAutoDelete(FALSE);
  colorButtonList.setAutoDelete(FALSE);
  intList.setAutoDelete(FALSE);
  doubleList.setAutoDelete(FALSE);
  checkList.setAutoDelete(FALSE);
  fontButtonList.setAutoDelete(FALSE);
  textList.setAutoDelete(FALSE);
  comboList.setAutoDelete(FALSE);
  dateList.setAutoDelete(FALSE);
  fileList.setAutoDelete(FALSE);
  symbolList.setAutoDelete(FALSE);
  dvList.setAutoDelete(FALSE);
  labelList.setAutoDelete(FALSE);
  timeList.setAutoDelete(FALSE);

  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  setHelpButton(tr("&Help"));

  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

void PrefDialog::createPage (QString &name)
{
  QWidget *w = new QWidget(this);
  widgetList.replace(name, w);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);

  Q3GridLayout *grid = new Q3GridLayout(vbox, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  gridList.replace(name, grid);
  grid->setColStretch(1, 1);

  vbox->insertStretch(-1, 1);

  addTab(w, name);
}

void PrefDialog::deletePage (QString &name)
{
  removePage(widgetList[name]);
}

void PrefDialog::setHelpFile (QString &d)
{
  helpFile = d;
}

void PrefDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void PrefDialog::addColorItem (QString &name, QString &page, QColor &color)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  ColorButton *button = new ColorButton(w, color);
  grid->addWidget(button, grid->numRows() - 2, 1);
  button->setColorButton();
  colorButtonList.replace(name, button);
}

void PrefDialog::addColorPrefItem (QString &name, QString &page, QColor &color)
{
  QWidget *w = widgetList[page];
  ColorButton *mainButton = new ColorButton(w, color);

  // add the preset preferred buttons
  // for now a fixed quantity of 5 pieces with fixed colors
  // may/will later expand with the possibility to make user defineable too
  QString prefButtonName = "PrefColor";
  ColorButton *prefButton;
  QColor prefColor[5];
  prefColor[0].setNamedColor("white");
  prefColor[1].setNamedColor("red");
  prefColor[2].setNamedColor("green");
  prefColor[3].setRgb(85,170,255); // something like blue
  prefColor[4].setRgb(255,170,0); // orange

  // create pref-buttons and arange in one row(hbox)
  Q3HBoxLayout *hbox = new Q3HBoxLayout(0,0,0,"hbox");
  int i;
  for (i = 0; i < 5; i++)
  {
    prefButton = new ColorButton(w, prefColor[i]);
    prefButton->setDialogOff();
    prefButton->pix.resize(10, 10);
    prefButton->setColorButton();
    connect (prefButton, SIGNAL(robPressed(QColor)), mainButton, SLOT(setColor(QColor)));
    hbox->addWidget(prefButton);
    colorButtonList.replace(prefButtonName + QString::number(i, 'f', 0), prefButton);

  }

  Q3VBoxLayout *vbox = new Q3VBoxLayout(0,0,0,"vbox");
  vbox->addLayout(hbox);
  vbox->addWidget(mainButton);

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  grid->addLayout(vbox, grid->numRows() - 2, 1);
  mainButton->setColorButton();
  colorButtonList.replace(name, mainButton);


}

void PrefDialog::getColor (QString &name, QColor &color)
{
  ColorButton *button = colorButtonList[name];
  if (button)
    button->getColor(color);
}

void PrefDialog::addDoubleItem (QString &name, QString &page, double num)
{
  addDoubleItem(name, page, num, -9999999999.0, 9999999999.0);
}

void PrefDialog::addDoubleItem (QString &name, QString &page, double num, double low, double high)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QDoubleValidator *dv = new QDoubleValidator(low, high, 4, this, 0);
  dvList.replace(name, dv);

  QLineEdit *edit = new QLineEdit(QString::number(num), w);
  edit->setValidator(dv);
  grid->addWidget(edit, grid->numRows() - 2, 1);
  doubleList.replace(name, edit);
}

double PrefDialog::getDouble (QString &name)
{
  double num = 0;
  QLineEdit *edit = doubleList[name];
  if (edit)
    num = edit->text().toDouble();
  return num;
}

void PrefDialog::addIntItem (QString &name, QString &page, int num)
{
  addIntItem(name, page, num, -999999999, 999999999);
}

void PrefDialog::addIntItem (QString &name, QString &page, int num, int min, int max)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QSpinBox *spin = new QSpinBox(min, max, 1, w);
  spin->setValue(num);
  grid->addWidget(spin, grid->numRows() - 2, 1);
  intList.replace(name, spin);
}

int PrefDialog::getInt (QString &name)
{
  int num = 0;
  QSpinBox *spin = intList[name];
  if (spin)
    num = spin->value();
  return num;
}

void PrefDialog::addCheckItem (QString &name, QString &page, QString &flag)
{
  if (! flag.compare("True"))
    addCheckItem(name, page, TRUE);
  else
    addCheckItem(name, page, FALSE);
}

void PrefDialog::addCheckItem (QString &name, QString &page, bool flag)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QCheckBox *check = new QCheckBox(w);
  check->setChecked(flag);
  grid->addWidget(check, grid->numRows() - 2, 1);
  checkList.replace(name, check);
}

bool PrefDialog::getCheck (QString &name)
{
  bool flag = FALSE;
  QCheckBox *check = checkList[name];
  if (check)
    flag = check->isChecked();
  return flag;
}

void PrefDialog::getCheckString (QString &name, QString &flag)
{
  flag.truncate(0);
  QCheckBox *check = checkList[name];
  if (check)
  {
    if (check->isChecked())
      flag = "True";
    else
      flag = "False";
  }
}

void PrefDialog::addFontItem (QString &name, QString &page, QFont &font)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  FontButton *button = new FontButton(w, font);
  grid->addWidget(button, grid->numRows() - 2, 1);
  fontButtonList.replace(name, button);
}

void PrefDialog::getFont (QString &name, QFont &font)
{
  FontButton *button = fontButtonList[name];
  if (button)
    button->getFont(font);
}

void PrefDialog::addTextItem (QString &name, QString &page, QString &t)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QLineEdit *edit = new QLineEdit(t, w);
  grid->addWidget(edit, grid->numRows() - 2, 1);
  textList.replace(name, edit);
}

void PrefDialog::getText (QString &name, QString &s)
{
  s.truncate(0);
  QLineEdit *edit = textList[name];
  if (edit)
    s = edit->text();
}

void PrefDialog::addComboItem (QString &name, QString &page, QStringList &l, QString &s)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QComboBox *combo = new QComboBox(w);
  combo->insertStringList(l, -1);
  if (s.length())
  {
    int t = l.findIndex(s);
    if (t != -1)
      combo->setCurrentItem(t);
  }
  grid->addWidget(combo, grid->numRows() - 2, 1);
  comboList.replace(name, combo);
}

void PrefDialog::addComboItem (QString &name, QString &page, QStringList &l, int s)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QComboBox *combo = new QComboBox(w);
  combo->insertStringList(l, -1);
  combo->setCurrentItem(s);
  grid->addWidget(combo, grid->numRows() - 2, 1);
  comboList.replace(name, combo);
}

void PrefDialog::getCombo (QString &name, QString &s)
{
  s.truncate(0);
  QComboBox *combo = comboList[name];
  if (combo)
    s = combo->currentText();
}

int PrefDialog::getComboIndex (QString &name)
{
  int i = 0;
  QComboBox *combo = comboList[name];
  if (combo)
    i = combo->currentItem();
  return i;
}

QComboBox * PrefDialog::getComboWidget (QString &name)
{
  return comboList[name];
}

void PrefDialog::addDateItem (QString &name, QString &page, QDateTime &dt)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  Q3DateEdit *date = new Q3DateEdit(dt.date(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(Q3DateEdit::YMD);
  grid->addWidget(date, grid->numRows() - 2, 1);
  dateList.replace(name, date);
}

void PrefDialog::getDate (QString &name, QDateTime &dt)
{
  Q3DateEdit *date = dateList[name];
  if (date)
    dt.setDate(date->date());
}

void PrefDialog::addTimeItem (QString &name, QString &page, QDateTime &dt)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  Q3TimeEdit *time = new Q3TimeEdit(dt.time(), w);
  time->setAutoAdvance(TRUE);
  grid->addWidget(time, grid->numRows() - 2, 1);
  timeList.replace(name, time);
}

void PrefDialog::getTime (QString &name, QDateTime &dt)
{
  Q3TimeEdit *time = timeList[name];
  if (time)
    dt.setTime(time->time());
}

void PrefDialog::addFileItem (QString &name, QString &page, QStringList &l, QString &p)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  FileButton *button = new FileButton(w, l, p);
  grid->addWidget(button, grid->numRows() - 2, 1);
  fileList.replace(name, button);
}

void PrefDialog::getFile (QString &name, QStringList &l)
{
  l.clear();
  FileButton *button = fileList[name];
  if (button)
    button->getFile(l);
}

void PrefDialog::addSymbolItem (QString &name, QString &page, QString &path, QString &symbol)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  SymbolButton *button = new SymbolButton(w, path, symbol);
  grid->addWidget(button, grid->numRows() - 2, 1);
  symbolList.replace(name, button);
}

void PrefDialog::getSymbol (QString &name, QString &s)
{
  s.truncate(0);
  SymbolButton *button = symbolList[name];
  if (button)
    button->getPath(s);
}

void PrefDialog::addLabelItem (QString &name, QString &page, QString &l)
{
  QWidget *w = widgetList[page];

  Q3GridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());

  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  label = new QLabel(l, w);
  label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  grid->addWidget(label, grid->numRows() - 2, 1);
  labelList.replace(name, label);
}

void PrefDialog::getItem (QString &name, QString &s)
{
  s.truncate(0);

  // check color
  QColor color;
  getColor(name, color);
  if (color.isValid())
  {
    s = color.name();
    return;
  }

  // check for double
  QLineEdit *edit = doubleList[name];
  if (edit)
  {
    s = edit->text();
    return;
  }

  // check for int
  QSpinBox *spin = intList[name];
  if (spin)
  {
    s = spin->text();
    return;
  }

  getText(name, s);
  if (s.length())
    return;

  QComboBox *combo = comboList[name];
  if (combo)
  {
    s = QString::number(combo->currentItem());
    return;
  }
}
