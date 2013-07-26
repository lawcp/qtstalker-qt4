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

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <qstring.h>
#include <q3tabdialog.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include "Config.h"
#include "FormulaEdit.h"
#include "DBIndex.h"

class Scanner : public Q3TabDialog
{
  Q_OBJECT

  signals:
    void exitScanner ();
    void message (QString);
    void scanComplete ();

  public:
    Scanner (QString, DBIndex *);
    ~Scanner ();

  public slots:
    void scan ();
    void saveRule ();
    void exitDialog ();
    void loadRule ();
    void getSymbols ();
    void allSymbolsToggled (bool);
    void slotHelp ();

  protected:
    FormulaEdit *list;
    QComboBox *period;
    QString scannerName;
    Config config;
    QPushButton *fileButton;
    QStringList fileList;
    QCheckBox *allSymbols;
    QStringList barLengthList;
    QSpinBox *bars;
    QComboBox *basePath;
    DBIndex *chartIndex;
};

#endif
