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

#include "HelpWindow.h"
#include "RcFile.h"
#include "../pics/next.xpm"
#include "../pics/home.xpm"
#include "../pics/previous.xpm"
#include "../pics/disable.xpm"
#include <qlayout.h>
#include <qdir.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

HelpWindow::HelpWindow (QWidget *w, QString &fn) : QDialog (w, "HelpWindow", FALSE, Qt::WDestructiveClose)
{
  RcFile rcfile;
  rcfile.loadData(RcFile::DocsPath, docsPath);
  docsPath.append("/");
  tocPath = docsPath + "toc.html";

  Q3VBoxLayout *vbox = new Q3VBoxLayout (this);
  vbox->setSpacing(5);
  vbox->setMargin(5);

  toolbar = new Toolbar(this, Toolbar::Horizontal);
  vbox->addWidget(toolbar);

  QString s("home");
  QString s2(tr("Home"));
  toolbar->addButton(s, home, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(goHome()));

  s = "previous";
  s2 = tr("Previous");
  toolbar->addButton(s, previous, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(goPrevious()));

  s = "next";
  s2 = tr("Next");
  toolbar->addButton(s, next, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(goNext()));

  s = "exit";
  s2 = tr("Close");
  toolbar->addButton(s, disable, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(exit()));

  text = new Q3TextBrowser(this);

  QObject::connect(text, SIGNAL(backwardAvailable(bool)), this, SLOT(previousStatus(bool)));
  QObject::connect(text, SIGNAL(forwardAvailable(bool)), this, SLOT(nextStatus(bool)));
  QObject::connect(text, SIGNAL(linkClicked(const QString &)), this, SLOT(slotLoadFile(const QString &)));

  vbox->addWidget (text);

  s = docsPath + fn;
  QDir dir;
  // If the doc exists at fn (perhaps full pathname) then use it
  // otherwise try relative to DocsFilePath, otherwise use the default
  if (dir.exists(fn) && s.contains(".html"))
    text->setSource(fn);
  else if (dir.exists(s) && s.contains(".html"))
    text->setSource(s);
  else
  {
    qDebug("Document not found: " + fn);
    text->setSource(tocPath);
  }

  setCaption(text->documentTitle());

  QSize sz;
  rcfile.loadSize(RcFile::MainWindowSize, sz);
  resize(sz - QSize(38, 48));

}

HelpWindow::~HelpWindow ()
{
}

void HelpWindow::goHome ()
{
  text->setSource(tocPath);
}

void HelpWindow::goPrevious ()
{
  text->backward();
}

void HelpWindow::goNext ()
{
  text->forward();
}

void HelpWindow::previousStatus (bool d)
{
  QString s("previous");
  toolbar->setButtonStatus(s, d);
}

void HelpWindow::nextStatus (bool d)
{
  QString s("next");
  toolbar->setButtonStatus(s, d);
}

void HelpWindow::exit ()
{
  RcFile rcfile;
  rcfile.saveSize(RcFile::HelpWindowSize, size());
  done(0);
}

void HelpWindow::slotLoadFile(const QString &fileName)
{
  if (fileName.contains(".html"))
  {
      text->setSource(fileName);
      //setCaption(text->documentTitle());
  }
  else
  {
      // In case to view a file without known mime source extension, e.g. CHANGELOG.0.3X
      QFile file(QFile::encodeName(fileName));
      QString s(file.name());

      if ( file.open( QIODevice::ReadOnly ) )
      {
        s = (QString)file.readAll();
        file.close();
        s.replace( QChar('<'), "&lt;" );
        s.replace( QChar('>'), "&gt;" );
        s.prepend("<html><title>CHANGELOG</title<body><pre>");
        // FIXME: generate <title> out of fileName
        // FIXME: add a "back" link or a complete list of all available CHANGLOG files
        //        possible in form of a left hand placed table
        s.append("</pre></body></html>");
      }
      else
      {
        s = file.errorString ();
      }
      text->setText(s);
      //setCaption(text->documentTitle());
  }
  setCaption(text->documentTitle());
}
