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

#include "FuturesDialog.h"
#include "Bar.h"
#include "HelpWindow.h"
#include "DBIndexItem.h"
#include "Exchange.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3VBoxLayout>

FuturesDialog::FuturesDialog (QString p, DbPlugin *d, DBIndex *i) : Q3TabDialog (0, "FuturesDialog", TRUE)
{
  helpFile = p;
  db = d;
  index = i;
  reloadFlag = FALSE;
  currentDate = QDateTime::currentDateTime();

  setCaption(tr("Qtstalker: Edit Futures"));

  QString s;
  d->getSymbol(s);
  QFileInfo fi(s);
  symbol = fi.fileName();

  createDetailsPage();
  createDataPage();

  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveChart()));
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

FuturesDialog::~FuturesDialog ()
{
}

void FuturesDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  Q3GridLayout *grid = new Q3GridLayout(vbox);
  grid->setSpacing(5);

  QLabel *label = new QLabel(tr("Symbol"), w);
  grid->addWidget(label, 0, 0);

  QString s;
  DBIndexItem item;
  index->getIndexItem(symbol, item);
  item.getSymbol(s);
  label = new QLabel(s, w);
  label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  grid->addWidget(label, 0, 1);

  label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 1, 0);

  item.getTitle(s);
  title = new QLineEdit(s, w);
  grid->addWidget(title, 1, 1);

  label = new QLabel(tr("Exchange"), w);
  grid->addWidget(label, 2, 0);

  item.getExchange(s);
  Exchange ex;
  ex.getExchange(s.toInt(), s);
  label = new QLabel(s, w);
  label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  grid->addWidget(label, 2, 1);

  label = new QLabel(tr("Type"), w);
  grid->addWidget(label, 3, 0);

  item.getType(s);
  label = new QLabel(s, w);
  label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  grid->addWidget(label, 3, 1);

  label = new QLabel(tr("Futures Type"), w);
  grid->addWidget(label, 4, 0);

  QString s2;
  item.getFuturesType(s2);
  label = new QLabel(s2, w);
  label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  grid->addWidget(label, 4, 1);

  label = new QLabel(tr("Futures Month"), w);
  grid->addWidget(label, 5, 0);

  item.getFuturesMonth(s2);
  label = new QLabel(s2, w);
  label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  grid->addWidget(label, 5, 1);

  label = new QLabel(tr("First Date"), w);
  grid->addWidget(label, 6, 0);

  Bar bar;
  db->getFirstBar(bar);
  if (! bar.getEmptyFlag())
  {
    bar.getDateTimeString(TRUE, s);
    label = new QLabel(s, w);
    label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    grid->addWidget(label, 6, 1);
  }

  label = new QLabel(tr("Last Date"), w);
  grid->addWidget(label, 7, 0);


  Bar bar2;
  db->getLastBar(bar2);
  if (! bar2.getEmptyFlag())
  {
    bar2.getDateTimeString(TRUE, s);
    label = new QLabel(s, w);
    label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    grid->addWidget(label, 7, 1);
  }

  grid->setColStretch(1, 1);
  vbox->insertStretch(-1, 0);

  addTab(w, tr("Details"));
}

void FuturesDialog::createDataPage ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  barEdit = new BarEdit(w);
  QString s = tr("Open");
  QString s2 = "Open";
  barEdit->createField(s, s2, FALSE);
  s = tr("High");
  s2 = "High";
  barEdit->createField(s, s2, FALSE);
  s = tr("Low");
  s2 = "Low";
  barEdit->createField(s, s2, FALSE);
  s = tr("Close");
  s2 = "Close";
  barEdit->createField(s, s2, FALSE);
  s = tr("Volume");
  s2 = "Volume";
  barEdit->createField(s, s2, FALSE);
  s = tr("OI");
  s2 = "OI";
  barEdit->createField(s, s2, TRUE);
  connect(barEdit, SIGNAL(signalDeleteRecord()), this, SLOT(deleteRecord()));
  connect(barEdit, SIGNAL(signalSaveRecord()), this, SLOT(saveRecord()));
  connect(barEdit, SIGNAL(signalSearch(QDateTime)), this, SLOT(slotDateSearch(QDateTime)));
  connect(barEdit, SIGNAL(signalFirstRecord()), this, SLOT(slotFirstRecord()));
  connect(barEdit, SIGNAL(signalLastRecord()), this, SLOT(slotLastRecord()));
  connect(barEdit, SIGNAL(signalPrevRecord()), this, SLOT(slotPrevRecord()));
  connect(barEdit, SIGNAL(signalNextRecord()), this, SLOT(slotNextRecord()));
  vbox->addWidget(barEdit);

  addTab(w, tr("Data"));
}

void FuturesDialog::deleteRecord ()
{
  Bar bar;
  QString s;
  barEdit->getDate(s);
  if (bar.setDate(s))
    return;
  bar.getDateTimeString(FALSE, s);
  db->deleteData(s);

  reloadFlag = TRUE;
}

void FuturesDialog::saveRecord ()
{
  Bar bar;
  QString s, s2;
  barEdit->getDate(s);
  bar.setDate(s);

  s = "Open";
  barEdit->getField(s, s2);
  bar.setOpen(s2.toDouble());

  s = "High";
  barEdit->getField(s, s2);
  bar.setHigh(s2.toDouble());

  s = "Low";
  barEdit->getField(s, s2);
  bar.setLow(s2.toDouble());

  s = "Close";
  barEdit->getField(s, s2);
  bar.setClose(s2.toDouble());

  s = "Volume";
  barEdit->getField(s, s2);
  bar.setVolume(s2.toDouble());

  s = "OI";
  barEdit->getField(s, s2);
  bar.setOI(s2.toInt());

  db->setBar(bar);

  reloadFlag = TRUE;
}

void FuturesDialog::slotDateSearch (QDateTime dt)
{
  Bar record;
  db->getSearchBar(dt, record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void FuturesDialog::saveChart ()
{
  QString s = title->text();
  DBIndexItem item;
  index->getIndexItem(symbol, item);
  item.setTitle(s);
  index->setIndexItem(symbol, item);

  if (barEdit->getSaveFlag())
  {
    int rc = QMessageBox::warning(this,
    			          tr("Warning"),
			          tr("Record has been modified.\nSave changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveRecord();
  }

  reloadFlag = TRUE;

  accept();
}

void FuturesDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void FuturesDialog::slotFirstRecord ()
{
  Bar record;
  db->getFirstBar(record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void FuturesDialog::slotLastRecord ()
{
  Bar record;
  db->getLastBar(record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void FuturesDialog::slotNextRecord ()
{
  Bar record;
  db->getNextBar(currentDate, record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void FuturesDialog::slotPrevRecord ()
{
  Bar record;
  db->getPrevBar(currentDate, record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void FuturesDialog::updateFields (Bar &record)
{
  record.getDate(currentDate);

  QString s;
  record.getDateTimeString(TRUE, s);
  barEdit->setDate(s, record.getTickFlag());

  s = "Open";
  QString s2 = QString::number(record.getOpen());
  barEdit->setField(s, s2);

  s = "High";
  s2 = QString::number(record.getHigh());
  barEdit->setField(s, s2);

  s = "Low";
  s2 = QString::number(record.getLow());
  barEdit->setField(s, s2);

  s = "Close";
  s2 = QString::number(record.getClose());
  barEdit->setField(s, s2);

  s = "Volume";
  s2 = QString::number(record.getVolume(), 'f', 0);
  barEdit->setField(s, s2);

  s = "OI";
  s2 = QString::number(record.getOI(), 'f', 0);
  barEdit->setField(s, s2);

  barEdit->clearButtons();
}

bool FuturesDialog::getReloadFlag ()
{
  return reloadFlag;
}
