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

#include "BarEdit.h"
#include "PrefDialog.h"
#include "../pics/deleteitem.xpm"
#include "../pics/exportfile.xpm"
#include "../pics/search.xpm"
#include "../pics/start.xpm"
#include "../pics/next.xpm"
#include "../pics/previous.xpm"
#include "../pics/end.xpm"
#include <qlabel.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>

BarEdit::BarEdit (QWidget *w) : QWidget (w)
{
  saveRecordFlag = FALSE;
  ignoreSaveRecordFlag = FALSE;
  editList.setAutoDelete(TRUE);
  saveLabel = "save";
  deleteLabel = "delete";
  QString searchLabel = "search";

  //QVBoxLayout *vbox = new QVBoxLayout(this);
  //vbox->setMargin(0);
  //vbox->setSpacing(0);

  //toolbar = new Toolbar(this, 30, 30, FALSE);
  toolbar = new Toolbar(this, Toolbar::Horizontal);
  //vbox->addWidget(toolbar);
  //vbox->addSpacing(10);

  QString s = tr("Search");
  toolbar->addButton(searchLabel, QPixmap(search), s);
  connect(toolbar->getButton(searchLabel), SIGNAL(clicked()), this, SLOT(slotDateSearch()));
  toolbar->getButton(searchLabel)->setAccel(Qt::CTRL+Qt::Key_R);

  s = tr("Save Record");
  toolbar->addButton(saveLabel, QPixmap(exportfile), s);
  connect(toolbar->getButton(saveLabel), SIGNAL(clicked()), this, SLOT(saveRecord()));
  toolbar->setButtonStatus(saveLabel, FALSE);
  toolbar->getButton(saveLabel)->setAccel(Qt::CTRL+Qt::Key_S);

  s = tr("Delete Record");
  toolbar->addButton(deleteLabel, QPixmap(deleteitem), s);
  connect(toolbar->getButton(deleteLabel), SIGNAL(clicked()), this, SLOT(deleteRecord()));
  toolbar->setButtonStatus(deleteLabel, FALSE);
  toolbar->getButton(deleteLabel)->setAccel(Qt::CTRL+Qt::Key_D);

  // setup the navigator area

//  toolbar->addSeparator();

  QString frLabel = "fr";
  s = tr("First Record");
  toolbar->addButton(frLabel, QPixmap(start), s);
  connect(toolbar->getButton(frLabel), SIGNAL(clicked()), this, SLOT(slotFirstRecord()));
//  toolbar->getButton(frLabel)->setAccel(Qt::CTRL+Qt::Key_D);

  QString prLabel = "pr";
  s = tr("Previous Record");
  toolbar->addButton(prLabel, QPixmap(previous), s);
  connect(toolbar->getButton(prLabel), SIGNAL(clicked()), this, SLOT(slotPrevRecord()));
//  toolbar->getButton(prLabel)->setAccel(Qt::CTRL+Qt::Key_D);

  QString nrLabel = "nr";
  s = tr("Next Record");
  toolbar->addButton(nrLabel, QPixmap(next), s);
  connect(toolbar->getButton(nrLabel), SIGNAL(clicked()), this, SLOT(slotNextRecord()));
//  toolbar->getButton(nrLabel)->setAccel(Qt::CTRL+Qt::Key_D);

  QString lrLabel = "lr";
  s = tr("Last Record");
  toolbar->addButton(lrLabel, QPixmap(end), s);
  connect(toolbar->getButton(lrLabel), SIGNAL(clicked()), this, SLOT(slotLastRecord()));
//  toolbar->getButton(lrLabel)->setAccel(Qt::CTRL+Qt::Key_D);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  grid = new Q3GridLayout(vbox, 1, 3); // one row, three cols
  grid->setColStretch(1, 1);  // only stretch the last cols
  grid->setColStretch(2, 1);

  grid->setSpacing(2);
  grid->addMultiCellWidget(toolbar, 0, 0, 0, 1); // span over tow cols
  grid->expand(grid->numRows() + 1, grid->numCols()); // new row

  QLabel *label = new QLabel(tr("Date"), this);
  date = new QLineEdit(this);
  date->setReadOnly(TRUE);

  grid->addWidget(label, grid->numRows() -1, 0); // add to last row
  grid->addMultiCellWidget(date, grid->numRows() -1, grid->numRows() -1, 1, 2); // span over last two cols


  grid->expand(grid->numRows() + 1, grid->numCols());


}

BarEdit::~BarEdit ()
{
}

void BarEdit::createField (QString &l, QString &d, bool f)
{
  QLabel *label = new QLabel(l, this);
  grid->addWidget(label, grid->numRows() - 1, 0);

  QLineEdit *edit = new QLineEdit(this);
  editList.replace(d, edit);

  if (f)
  {
    QIntValidator *iv = new QIntValidator(0, 999999999, this);
    edit->setValidator(iv);
    connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    //grid->addWidget(edit, grid->numRows() - 1, 1);
    grid->addMultiCellWidget(edit, grid->numRows() -1, grid->numRows() -1, 1, 2);
  }
  else
  {
    QDoubleValidator *dv = new QDoubleValidator(0, 99999999999.0, 4, this, 0);
    edit->setValidator(dv);
    connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    //grid->addWidget(edit, grid->numRows() - 1, 1);
    grid->addMultiCellWidget(edit, grid->numRows() -1, grid->numRows() -1, 1, 2);
  }

  grid->expand(grid->numRows() + 1, grid->numCols());
}

void BarEdit::deleteRecord ()
{
  int rc = QMessageBox::warning(this,
  			        tr("Delete record."),
			        tr("Are you sure to delete this record?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  emit signalDeleteRecord();

  clearRecordFields();

  toolbar->setButtonStatus(deleteLabel, FALSE);
  toolbar->setButtonStatus(saveLabel, FALSE);

  saveRecordFlag = FALSE;
}

void BarEdit::saveRecord ()
{
  emit signalSaveRecord();
  toolbar->setButtonStatus(saveLabel, FALSE);
  saveRecordFlag = FALSE;
}

void BarEdit::slotDateSearch ()
{
  saveRecordDialog();

  QString pl = QObject::tr("Parms");
  QString dl = QObject::tr("Date");
  QString tl = QObject::tr("Time");
  QDateTime dt = QDateTime::currentDateTime();

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(QObject::tr("Date Search"));
  dialog->createPage (pl);
  dialog->addDateItem(dl, pl, dt);
  dialog->addTimeItem(tl, pl, dt);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  dialog->getDate(dl, dt);
  dialog->getTime(tl, dt);
  delete dialog;

  clearRecordFields();

  emit signalSearch(dt);

  toolbar->setButtonStatus(deleteLabel, FALSE);
  toolbar->setButtonStatus(saveLabel, FALSE);
}

void BarEdit::clearRecordFields ()
{
  ignoreSaveRecordFlag = TRUE;

  date->clear();

  Q3DictIterator<QLineEdit> it(editList);
  for(; it.current(); ++it)
    it.current()->clear();

  ignoreSaveRecordFlag = FALSE;

  saveRecordFlag = FALSE;
}

void BarEdit::textChanged (const QString &)
{
  if (! ignoreSaveRecordFlag)
  {
    saveRecordFlag = TRUE;
    toolbar->setButtonStatus(saveLabel, TRUE);
  }
}

void BarEdit::setField (QString &k, QString &d)
{
  ignoreSaveRecordFlag = TRUE;

  QLineEdit *edit = editList[k];
  if (edit)
    edit->setText(d);

  ignoreSaveRecordFlag = FALSE;
}

void BarEdit::setDate (QString &d, bool)
{
  date->setText(d);
}

void BarEdit::getField (QString &d, QString &r)
{
  r.truncate(0);;
  QLineEdit *edit = editList[d];
  if (edit)
    r = edit->text();
}

void BarEdit::getDate (QString &r)
{
  r = date->text();
}

bool BarEdit::getSaveFlag ()
{
  return saveRecordFlag;
}

void BarEdit::slotFirstRecord ()
{
  saveRecordDialog();
  emit signalFirstRecord();
}

void BarEdit::slotLastRecord ()
{
  saveRecordDialog();
  emit signalLastRecord();
}

void BarEdit::slotNextRecord ()
{
  saveRecordDialog();
  emit signalNextRecord();
}

void BarEdit::slotPrevRecord ()
{
  saveRecordDialog();
  emit signalPrevRecord();
}

void BarEdit::saveRecordDialog ()
{
  if (! saveRecordFlag)
    return;

  int rc = QMessageBox::warning(this,
		                tr("Warning"),
		                tr("Record has been modified.\nSave changes?"),
		                QMessageBox::Yes,
		                QMessageBox::No,
		                QMessageBox::NoButton);

  if (rc == QMessageBox::Yes)
    saveRecord();
  else
  {
    saveRecordFlag = FALSE;
    toolbar->setButtonStatus(saveLabel, FALSE);
  }
}

void BarEdit::clearButtons ()
{
  toolbar->setButtonStatus(saveLabel, FALSE);
  toolbar->setButtonStatus(deleteLabel, TRUE);
}
