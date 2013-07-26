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

#include "StocksDialog.h"
#include "Bar.h"
#include "HelpWindow.h"
#include "DBIndexItem.h"
#include "COBase.h"
#include "Exchange.h"
#include "RcFile.h"
#include <qsize.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3VBoxLayout>

StocksDialog::StocksDialog (QString p, DbPlugin *d, DBIndex *i) : Q3TabDialog (0, "StocksDialog", TRUE)
{
  helpFile = p;
  db = d;
  index = i;
  reloadFlag = FALSE;
  currentDate = QDateTime::currentDateTime();

  setCaption(tr("Qtstalker: Edit Stock"));

  QString s;
  d->getSymbol(s);
  QFileInfo fi(s);
  symbol = fi.fileName();

  createDetailsPage();
  createDataPage();
  createFundamentalsPage();
  createSplitPage();

  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveChart()));

  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  RcFile rcfile;
  QSize sz;
  rcfile.loadSize(RcFile::EditStockWindowSize, sz);
  resize(sz);
}

StocksDialog::~StocksDialog ()
{
  RcFile rcfile;
  rcfile.saveSize(RcFile::EditStockWindowSize, size());
}

void StocksDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  Q3GridLayout *grid = new Q3GridLayout(vbox);
  grid->setMargin(0);
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

  label = new QLabel(tr("First Date"), w);
  grid->addWidget(label, 4, 0);

  Bar bar;
  db->getFirstBar(bar);
  if (! bar.getEmptyFlag())
  {
    bar.getDateTimeString(TRUE, s);
    label = new QLabel(s, w);
    label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    grid->addWidget(label, 4, 1);
  }

  label = new QLabel(tr("Last Date"), w);
  grid->addWidget(label, 5, 0);

  Bar bar2;
  db->getLastBar(bar2);
  if (! bar2.getEmptyFlag())
  {
    bar2.getDateTimeString(TRUE, s);
    label = new QLabel(s, w);
    label->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    grid->addWidget(label, 5, 1);
  }

  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);

  vbox->addStretch(1);

  addTab(w, tr("Details"));
}

void StocksDialog::createDataPage ()
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

void StocksDialog::createFundamentalsPage ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  Setting fund;
  QString s, s2;
  index->getFundamentals(symbol, s2);
  fund.parse(s2);

  s = tr("Fundamentals: last updated ");
  s2 = "updateDate";
  QString s3;
  fund.getData(s2, s3);
  s.append(s3);
  fund.remove(s2);
  QStringList key;
  fund.getKeyList(key);
  key.sort();

  vbox->addSpacing(10);
  QLabel *label = new QLabel(s, w);
  vbox->addWidget(label);

  fundView = new Q3ListView(w);
  fundView->addColumn(tr("Description"));
  fundView->addColumn(tr("Value"));
  vbox->addWidget(fundView);

  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
  {
    fund.getData(key[loop], s);
    new Q3ListViewItem(fundView, key[loop], s);
  }

  if (! key.count())
    new Q3ListViewItem(fundView, tr("No data available."));

  addTab(w, tr("Fundamentals"));
}

void StocksDialog::createSplitPage ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  Q3GridLayout *grid = new Q3GridLayout(vbox);
  grid->setMargin(0);
  grid->setSpacing(5);

  QLabel *label = new QLabel(tr("Split Date"), w);
  grid->addWidget(label, 0, 0);

  Bar bar;
  db->getLastBar(bar);
  QDateTime dt;
  bar.getDate(dt);
  if (! bar.getEmptyFlag())
    splitDate = new Q3DateEdit(dt.date(), w);
  else
    splitDate = new Q3DateEdit(QDate::currentDate(), w);
  splitDate->setOrder(Q3DateEdit::YMD);
  grid->addWidget(splitDate, 0, 1);

  label = new QLabel(tr("Split Ratio"), w);
  grid->addWidget(label, 1, 0);

  splitRatio = new QLineEdit("2:1", w);
  grid->addWidget(splitRatio, 1, 1);

  QPushButton *button = new QPushButton(tr("Perform Split"), w);
  connect(button, SIGNAL(clicked()), this, SLOT(split()));
  vbox->addWidget(button);

  vbox->addStretch(1);

  addTab(w, tr("Split"));
}

void StocksDialog::deleteRecord ()
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

void StocksDialog::saveRecord ()
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
  db->setBar(bar);

  reloadFlag = TRUE;
}

void StocksDialog::slotDateSearch (QDateTime dt)
{
  Bar record;
  db->getSearchBar(dt, record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void StocksDialog::saveChart ()
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

void StocksDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void StocksDialog::split ()
{
  int rc = QMessageBox::warning(this,
		                tr("Warning"),
			        tr("Are you sure to split this stock?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  QDate dt = splitDate->date();

  // verify if split date < first bar
  Bar bar;
  db->getFirstBar(bar);
  if (! bar.getEmptyFlag())
  {
    QDateTime td;
    bar.getDate(td);
    if (dt < td.date())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Invalid split date."));
      return;
    }
  }

  // verify if split date > last bar
  Bar bar2;
  db->getLastBar(bar2);
  if (! bar.getEmptyFlag())
  {
    QDateTime td;
    bar2.getDate(td);
    if (dt > td.date())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Invalid split date."));
      return;
    }
  }

  // verify if the ratio format is correct ?:?
  QStringList l = QStringList::split(":", splitRatio->text(), FALSE);
  if (l.count() != 2)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Invalid split ratio format.\neg. 2:1"));
    return;
  }

  double plyer = l[1].toDouble() / l[0].toDouble();
  double volplyer = l[0].toDouble() / l[1].toDouble();

  QString s;
  db->getSymbol(s);
  BarData *bars = new BarData(s);
  db->getAllBars(bars);

  int loop;
  for (loop = 0; loop < bars->count(); loop++)
  {
    Bar bar;
    bars->getBar(loop, bar);

    QDateTime td;
    bar.getDate(td);
    if (td.date() < dt)
    {
      bar.setOpen(bar.getOpen() * plyer);
      bar.setHigh(bar.getHigh() * plyer);
      bar.setLow(bar.getLow() * plyer);
      bar.setClose(bar.getClose() * plyer);
      bar.setVolume(bar.getVolume() * volplyer);
      db->setBar(bar);
    }
  }

  delete bars;

  // adjust any chart objects
  QDateTime adt(dt, QTime(0,0,0,0));
  QString fn;
  db->getIndexKey(fn);
  index->getChartObjects(fn, l);
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting set;
    set.parse(l[loop]);
    COBase tco;
    COBase *co = tco.getCO(set);
    if (! co)
      continue;

    co->adjustForSplit(adt, plyer);

    set.clear();
    co->getSettings(set);
    s = "Name";
    QString s2;
    set.getData(s, s2);
    index->setChartObject(fn, s2, set);
  }

  QMessageBox::information(this, tr("Qtstalker: Split Complete"), tr("Split complete."));

  reloadFlag = TRUE;
}

void StocksDialog::slotFirstRecord ()
{
  Bar record;
  db->getFirstBar(record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void StocksDialog::slotLastRecord ()
{
  Bar record;
  db->getLastBar(record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void StocksDialog::slotNextRecord ()
{
  Bar record;
  db->getNextBar(currentDate, record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void StocksDialog::slotPrevRecord ()
{
  Bar record;
  db->getPrevBar(currentDate, record);
  if (record.getEmptyFlag())
    return;
  updateFields(record);
}

void StocksDialog::updateFields (Bar &record)
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

  barEdit->clearButtons();
}

bool StocksDialog::getReloadFlag ()
{
  return reloadFlag;
}
