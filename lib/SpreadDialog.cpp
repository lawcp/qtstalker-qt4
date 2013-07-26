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

#include "SpreadDialog.h"
#include "HelpWindow.h"
#include "Config.h"
#include "DbPlugin.h"
#include <qlayout.h>
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlabel.h>

SpreadDialog::SpreadDialog (QString &nam, QString &fs, QString &ss) : QTabDialog (0, "SpreadDialog", TRUE)
{
  helpFile = "spread.html";

  setCaption(tr("Spread Dialog"));

  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 0, 0);

  name = new QLineEdit(nam, w);
  grid->addWidget(name, 0, 1);

  label = new QLabel(tr("First Symbol"), w);
  grid->addWidget(label, 1, 0);

  Config config;
  QString path;
  config.getData(Config::DataPath, path);

  firstSymbol = new SymbolButton(w, path, fs);
  grid->addWidget(firstSymbol, 1, 1);

  label = new QLabel(tr("Second Symbol"), w);
  grid->addWidget(label, 2, 0);

  secondSymbol = new SymbolButton(w, path, ss);
  grid->addWidget(secondSymbol, 2, 1);

  vbox->insertStretch(-1, 1);

  addTab(w, tr("Settings"));

  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  resize(640, 480);
}

SpreadDialog::SpreadDialog () : QTabDialog (0, "SpreadDialog", TRUE)
{
}

SpreadDialog::~SpreadDialog ()
{
}

void SpreadDialog::newSpread ()
{
  bool ok = FALSE;
  QString spread = QInputDialog::getText(QObject::tr("New Spread"),
                                         QObject::tr("Enter symbol name for the new Spread"),
                                         QLineEdit::Normal,
                                         QString::null,
                                         &ok,
                                         0);
  if (! spread.length() || ok == FALSE)
    return;

  QDir dir;
  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/Spread");
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
                               QObject::tr("Could not create Spread directory."));
      return;
    }
  }

  s.append("/" + spread);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
                             QObject::tr("This Spread already exists."));
    return;
  }

  DbPlugin db;
  if (db.openChart(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Disk Error"),
                             QObject::tr("Cannot create chart."));
    db.close();
    return;
  }

  db.setHeaderField(DbPlugin::Symbol, spread);
  s = "Spread";
  db.setHeaderField(DbPlugin::Type, s);
  db.setHeaderField(DbPlugin::Plugin, s);
  db.setHeaderField(DbPlugin::Title, spread);
  db.close();
}

void SpreadDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void SpreadDialog::getName (QString &d)
{
  d = name->text();
}

void SpreadDialog::getFirstSymbol (QString &d)
{
  firstSymbol->getPath(d);
}

void SpreadDialog::getSecondSymbol (QString &d)
{
  secondSymbol->getPath(d);
}
