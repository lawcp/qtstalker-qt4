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

#ifndef SPREADDIALOG_HPP
#define SPREADDIALOG_HPP

#include <qtabdialog.h>
#include <qstring.h>
#include <qlineedit.h>
#include "SymbolButton.h"

class SpreadDialog : public QTabDialog
{
  Q_OBJECT

  public:
    SpreadDialog (QString &, QString &, QString &);
    SpreadDialog ();
    ~SpreadDialog ();
    void newSpread ();
    void getName (QString &);
    void getFirstSymbol (QString &);
    void getSecondSymbol (QString &);

  public slots:
    void help ();

  private:
    QString helpFile;
    QLineEdit *name;
    SymbolButton *firstSymbol;
    SymbolButton *secondSymbol;
};

#endif
