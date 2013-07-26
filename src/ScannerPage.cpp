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

#include "ScannerPage.h"
#include "Scanner.h"
#include "SymbolDialog.h"
#include "HelpWindow.h"
#include "../pics/help.xpm"
#include "../pics/openitem.xpm"
#include "../pics/newchart.xpm"
#include "../pics/deleteitem.xpm"
#include "../pics/renameitem.xpm"
#include "../pics/macro.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <q3accel.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include <Q3VBoxLayout>

ScannerPage::ScannerPage (QWidget *w, DBIndex *i) : QWidget (w)
{
  chartIndex = i;
  idir.setFilter(QDir::Files);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(5);

  search = new QLineEdit(this);
  search->setText("*");
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  list = new Q3ListBox(this);
  connect(list, SIGNAL(contextMenuRequested(Q3ListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(Q3ListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(scannerSelected(const QString &)));
  connect(list, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(doubleClick(Q3ListBoxItem *)));
  vbox->addWidget(list);

  menu = new QMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Scanner		Ctrl+N"), this, SLOT(newScanner()));
  menu->insertItem(QPixmap(openitem), tr("&Open Scanner		Ctrl+O"), this, SLOT(openScanner()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Scanner	Ctrl+D"), this, SLOT(deleteScanner()));
  menu->insertItem(QPixmap(renameitem), tr("&Rename Scanner		Ctrl+R"), this, SLOT(renameScanner()));
  menu->insertItem(QPixmap(macro), tr("R&un Scanner		Ctrl+U"), this, SLOT(runScanner()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  Q3Accel *a = new Q3Accel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(Qt::CTRL+Qt::Key_N, NewScanner);
  a->insertItem(Qt::CTRL+Qt::Key_O, OpenScanner);
  a->insertItem(Qt::CTRL+Qt::Key_D, DeleteScanner);
  a->insertItem(Qt::CTRL+Qt::Key_R, RenameScanner);
  a->insertItem(Qt::CTRL+Qt::Key_U, RunScanner);
  a->insertItem(Qt::CTRL+Qt::Key_H, Help);

  refreshList();
  scannerSelected(QString());
}

ScannerPage::~ScannerPage ()
{
}

void ScannerPage::openScanner ()
{
  Scanner *dialog = new Scanner(list->currentText(), chartIndex);
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  connect(dialog, SIGNAL(scanComplete()), this, SIGNAL(refreshGroup()));
  dialog->show();
}

void ScannerPage::openScanner (QString d)
{
  Scanner *dialog = new Scanner(d, chartIndex);
  connect(dialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
  connect(dialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
  connect(dialog, SIGNAL(scanComplete()), this, SIGNAL(refreshGroup()));
  dialog->show();
}

void ScannerPage::runScanner ()
{
  QString s;
  config.getData(Config::ScannerPath, s);
  QString s2("*");
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s,
  					  s,
					  s2,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select scanners to run"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList l = dialog->selectedFiles();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QFileInfo fi(l[loop]);
      Scanner *sdialog = new Scanner(fi.fileName(), chartIndex);
      connect(sdialog, SIGNAL(exitScanner()), this, SLOT(refreshList()));
      connect(sdialog, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
      sdialog->show();
      sdialog->scan();
      delete sdialog;
    }
  }

  delete dialog;
}

void ScannerPage::newScanner()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Scanner"),
  				    tr("Enter new scanner name."),
				    QLineEdit::Normal,
				    tr("NewScanner"),
				    &ok,
				    this);
  if ((ok) && (! s.isNull()))
  {
    int loop;
    QString selection;
    for (loop = 0; loop < (int) s.length(); loop++)
    {
      QChar c = s.at(loop);
      if (c.isLetterOrNumber())
        selection.append(c);
    }

    config.getData(Config::ScannerPath, s);
    s.append("/" + selection);
    QDir dir(s);
    if (dir.exists(s))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This scanner already exists."));
      return;
    }

    openScanner(selection);
  }
}

void ScannerPage::deleteScanner()
{
  QString s;
  config.getData(Config::ScannerPath, s);
  QString s2("*");
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s,
  					  s,
					  s2,
					  Q3FileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Scanners To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure to delete selected scanners?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
      dir.remove(l[loop]);

    refreshList();
    scannerSelected(QString());
  }

  delete dialog;
}

void ScannerPage::renameScanner ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("Rename Scanner"),
  				    tr("Enter new scanner name."),
				    QLineEdit::Normal,
				    list->currentText(),
				    &ok,
				    this);
  if ((ok) && (! s.isNull()))
  {
    int loop;
    QString selection;
    for (loop = 0; loop < (int) s.length(); loop++)
    {
      QChar c = s.at(loop);
      if (c.isLetterOrNumber())
        selection.append(c);
    }

    config.getData(Config::ScannerPath, s);
    s.append("/" + selection);
    QDir dir(s);
    if (dir.exists(s))
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This scanner already exists."));
      return;
    }

    QString s2;
    config.getData(Config::ScannerPath, s2);
    s2.append("/" + list->currentText());

    dir.rename(s2, s);

    refreshList();
    scannerSelected(QString());
  }
}

void ScannerPage::scannerSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
    menu->setItemEnabled(menu->idAt(3), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
  }
}

void ScannerPage::rightClick (Q3ListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void ScannerPage::refreshList ()
{
  list->clear();

  QString s;
  config.getData(Config::ScannerPath, s);
  idir.setPath(s);
  int loop;
  for (loop = 0; loop < (int) idir.count(); loop++)
    list->insertItem(idir[loop], -1);
}

void ScannerPage::doubleClick (Q3ListBoxItem *item)
{
  if (! item)
    return;

  openScanner(item->text());
}

void ScannerPage::slotHelp ()
{
  QString s = "workwithscanner.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void ScannerPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);
}

void ScannerPage::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_N:
        slotAccel(NewScanner);
	break;
      case Qt::Key_D:
        slotAccel(DeleteScanner);
	break;
      case Qt::Key_O:
        slotAccel(OpenScanner);
	break;
      case Qt::Key_R:
        slotAccel(RenameScanner);
	break;
      default:
        break;
    }
  }
  else
  {
    switch (key->key())
    {
      case Qt::Key_Delete:
        deleteScanner();
        break;
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        break;
      case Qt::Key_Enter:
      case Qt::Key_Return:
        openScanner();
        break;
      default:
//        list->keyPressEvent(key);
        break;
    }
  }
}

void ScannerPage::slotAccel (int id)
{
  switch (id)
  {
    case NewScanner:
      newScanner();
      break;
    case DeleteScanner:
      deleteScanner();
      break;
    case RenameScanner:
      renameScanner();
      break;
    case OpenScanner:
      openScanner();
      break;
    case RunScanner:
      runScanner();
      break;
    case Help:
      slotHelp();
      break;
    default:
      break;
  }
}

void ScannerPage::searchChanged (const QString &d)
{
  idir.setNameFilter(d);
  refreshList();
}
