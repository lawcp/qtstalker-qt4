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

#ifndef SCANNERPAGE_HPP
#define SCANNERPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qmenu.h>
#include <q3listbox.h>
#include <qlineedit.h>
#include <qdir.h>
//Added by qt3to4:
#include <QKeyEvent>
#include "Config.h"
#include "DBIndex.h"


class ScannerPage : public QWidget
{
  Q_OBJECT

  signals:
    void message (QString);
    void refreshGroup ();

  public:

    enum HotKey
    {
      NewScanner,
      OpenScanner,
      DeleteScanner,
      RenameScanner,
      RunScanner,
      Help
    };

    ScannerPage (QWidget *, DBIndex *);
    ~ScannerPage ();

  public slots:
    void openScanner ();
    void openScanner (QString);
    void renameScanner ();
    void newScanner ();
    void deleteScanner ();
    void runScanner ();
    void scannerSelected (const QString &);
    void rightClick (Q3ListBoxItem *);
    void refreshList ();
    void slotHelp ();
    void doubleClick (Q3ListBoxItem *);
    void doKeyPress (QKeyEvent *);
    void slotAccel (int);
    void searchChanged (const QString &);

  private:
    virtual void keyPressEvent (QKeyEvent *);

    Config config;
    QMenu *menu;
    Q3ListBox *list;
    QLineEdit *search;
    QDir idir;
    DBIndex *chartIndex;
};

#endif
