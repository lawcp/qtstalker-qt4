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

#ifndef INDICATORPAGE_HPP
#define INDICATORPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qmenu.h>
#include <qdir.h>
#include <qcombobox.h>
#include <q3listbox.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QKeyEvent>
#include "DBIndex.h"
#include "Setting.h"
#include "Indicator.h"
#include "RcFile.h"


class IndicatorPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalDisableIndicator (QString);
    void signalEnableIndicator (QString);
    void signalEditIndicator (Indicator *);
    void signalDeleteIndicator (QString);
    void signalNewIndicator (Indicator *);
    void signalGroupChanged ();
    void signalGroupIsChanging ();
    void signalLocalIndicator (QString);

  public:

    enum HotKey
    {
      NewIndicatorGroup,
      DeleteIndicatorGroup,
      NewIndicator,
      AddLocalIndicator,
      DeleteIndicator,
      EditIndicator,
      MoveIndicator,
      Help,
      Tab
    };

    IndicatorPage (QWidget *, DBIndex *);
    ~IndicatorPage ();
    void updateGroups ();
    QString getIndicatorGroup ();
    void setFocus ();
    void setChartPath (QString &);
    void removeLocalIndicators ();
    void addLocalIndicators (QString &);

  public slots:
    void doubleClick (Q3ListBoxItem *);
    void updateList ();
    void slotHelp ();
    void rightClick (Q3ListBoxItem *);
    void newIndicator ();
    void editIndicator (QString);
    void editIndicator ();
    void deleteIndicator ();
    void itemSelected (const QString &);
    void changeIndicator (QString &);
    void slotAccel (int);
    void doKeyPress (QKeyEvent *key);
    void moveIndicator ();
    void slotGroupChanged (int);
    void newIndicatorGroup ();
    void deleteIndicatorGroup ();
    void searchChanged (const QString &);
    void addLocalIndicator ();

  protected:
    virtual void keyPressEvent (QKeyEvent *);

    Q3ListBox *list;
    QComboBox *group;
    QMenu *menu;
    QString baseDir;
    QString currentGroup;
    bool updateEnableFlag;
    QString chartPath;
    QStringList localIndicators;
    QLineEdit *search;
    QDir idir;
    DBIndex *chartIndex;
    RcFile rcfile;
};

#endif
