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

#ifndef SYMBOLDIALOG_HPP
#define SYMBOLDIALOG_HPP

#include <q3filedialog.h>
#include <qstring.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QKeyEvent>

class SymbolDialog : public Q3FileDialog
{
  Q_OBJECT

  signals:
    void signalKeyPressed(int, int, int, int, QString);

  public:
    SymbolDialog(QWidget *, QString &, QString &, QString &, Q3FileDialog::Mode);
    ~SymbolDialog();
    void setType(int);

  public slots:
    void dirSelected(const QString &);
    void setKeyFlag(bool);
    void doKeyPress(QKeyEvent *);

  protected:
    virtual void keyPressEvent(QKeyEvent *);

  private:
    QString basePath;
    bool keyFlag;
    int type;
};

#endif
