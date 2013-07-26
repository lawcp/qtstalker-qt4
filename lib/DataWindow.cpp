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

#include "DataWindow.h"
#include <qlayout.h>
#include "RcFile.h"
//Added by qt3to4:
#include <Q3VBoxLayout>

DataWindow::DataWindow (QWidget *w) : QDialog (w, "DataWindow", FALSE, Qt::WDestructiveClose)
{
  RcFile rcfile;
  QSize sz;
  rcfile.loadSize(RcFile::MainWindowSize, sz);
  resize(sz - QSize(38, 48));

  Q3VBoxLayout *vbox = new Q3VBoxLayout (this);
  vbox->setSpacing(5);
  vbox->setMargin(5);

  table = new Q3Table(this);
  table->setSelectionMode(Q3Table::Single);
  table->setReadOnly(TRUE);
  hHeader = table->horizontalHeader();
  vbox->addWidget (table);
}

DataWindow::~DataWindow ()
{
}

void DataWindow::setData (int row, int col, QString &data)
{
  if (row > table->numRows() -1)
    table->setNumRows(row + 1);
  table->setText(row, col, data);
}

void DataWindow::setHeader (int col, QString &d)
{
  if (col >= table->numCols())
    table->setNumCols(table->numCols() + 1);
  table->setColumnWidth(col, 80);
  hHeader->setLabel(col, d);
}

void DataWindow::setBars (BarData *d)
{
  if (! d->count())
    return;

  table->setNumCols(6);
  table->setNumRows(d->count());

  hHeader->setLabel(0, tr("Date"));
  hHeader->setLabel(1, tr("Time"));
  hHeader->setLabel(2, tr("Open"));
  hHeader->setLabel(3, tr("High"));
  hHeader->setLabel(4, tr("Low"));
  hHeader->setLabel(5, tr("Close"));

  int loop;
  for (loop = 0; loop < (int) d->count(); loop++)
  {
    QDateTime dt;
    d->getDate(loop, dt);
    QString s = dt.toString("yyyy-MM-dd");
    table->setText(loop, 0, s);
    s = dt.toString("hh:mm:ss");
    table->setText(loop, 1, s);
    table->setText(loop, 2, strip(d->getOpen(loop), 4));
    table->setText(loop, 3, strip(d->getHigh(loop), 4));
    table->setText(loop, 4, strip(d->getLow(loop), 4));
    table->setText(loop, 5, strip(d->getClose(loop), 4));
  }

  for (loop = 0; loop < table->numCols(); loop++)
    table->adjustColumn(loop);
}

void DataWindow::setPlot (Plot *d)
{
  Indicator *i = d->getIndicator();
  int loop2;
  QString s;
  for (loop2 = 0; loop2 < i->getLines(); loop2++)
  {
    table->setNumCols(table->numCols() + 1);

    PlotLine *line = i->getLine(loop2);
    line->getLabel(s);
    hHeader->setLabel(table->numCols() - 1, s);

    int loop3;
    int offset = table->numRows() - line->getSize();
    for (loop3 = 0; loop3 < line->getSize(); loop3++)
      table->setText(loop3 + offset, table->numCols() - 1, strip(line->getData(loop3), 4));
    table->adjustColumn(table->numCols() - 1);
  }
}

QString DataWindow::strip (double d, int p)
{
  QString s = QString::number(d, 'f', p);

  while (1)
  {
    if (s.find('.', -1, TRUE) != -1)
    {
      s.truncate(s.length() - 1);
      break;
    }
    else
    {
      if (s.find('0', -1, TRUE) != -1)
        s.truncate(s.length() - 1);
      else
        break;
    }
  }

  return s;
}
