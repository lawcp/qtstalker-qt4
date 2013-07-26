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

#ifndef BAREDIT_HPP
#define BAREDIT_HPP

//#include <qdatetimeedit.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include "Toolbar.h"

class BarEdit : public QWidget
{
  Q_OBJECT

  signals:
    void signalDeleteRecord();
    void signalSaveRecord();
    void signalFirstRecord();
    void signalLastRecord();
    void signalPrevRecord();
    void signalNextRecord();
    void signalSearch(QDateTime);

  public:
    BarEdit(QWidget *);
    ~BarEdit();
    void clearRecordFields();
    void createField(QString &, QString &, bool);
    void getDate(QString &);
    void setDate(QString &, bool);
    void setField(QString &, QString &);
    void getField(QString &, QString &);
    bool getSaveFlag();
    void saveRecordDialog();
    void clearButtons();

  public slots:
    void deleteRecord();
    void saveRecord();
    void slotDateSearch();
    void textChanged(const QString &);
    void slotFirstRecord();
    void slotLastRecord();
    void slotPrevRecord();
    void slotNextRecord();

  private:
    Q3GridLayout *grid;
//    QDateTimeEdit *dateSearch;
    QLineEdit *date;
    Q3Dict<QLineEdit> editList;
    Toolbar *toolbar;
    bool saveRecordFlag;
    bool ignoreSaveRecordFlag;
    QString saveLabel;
    QString deleteLabel;
};

#endif
