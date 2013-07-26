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

#ifndef FUTURESDIALOG_HPP
#define FUTURESDIALOG_HPP

#include <q3tabdialog.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include "DbPlugin.h"
#include "BarEdit.h"
#include "DBIndex.h"

class FuturesDialog : public Q3TabDialog
{
  Q_OBJECT

  public:
    FuturesDialog(QString, DbPlugin *, DBIndex *);
    ~FuturesDialog();
    void createDetailsPage();
    void createDataPage();
    void updateFields(Bar &);
    bool getReloadFlag();

  public slots:
    void deleteRecord();
    void saveRecord();
    void slotDateSearch(QDateTime);
    void saveChart();
    void help();
    void slotFirstRecord();
    void slotLastRecord();
    void slotPrevRecord();
    void slotNextRecord();

  private:
    DbPlugin *db;
    QLineEdit *title;
    QString helpFile;
    BarEdit *barEdit;
    QDateTime currentDate;
    DBIndex *index;
    QString symbol;
    bool reloadFlag;
};

#endif
