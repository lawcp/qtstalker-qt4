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

#include "UpgradeMessage.h"
#include "Config.h"
#include "DbPlugin.h"
#include "Traverse.h"
#include <qlayout.h>
#include <qwidget.h>
#include <qstring.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qfile.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <Q3TextStream>
#include <db.h>

UpgradeMessage::UpgradeMessage (int type) : Q3TabDialog (0, "UpgradeMessage", TRUE)
{
  switch (type)
  {
    case V031:
      createPage031();
      break;
    case V034:
      createPage034();
      break;
    default:
      break;
  }
}

UpgradeMessage::~UpgradeMessage ()
{
}

void UpgradeMessage::createPage031 ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QString s = tr("This version of Qtstalker uses a new data format.");
  s.append(tr(" It can not read data files from previous versions."));
  s.append(tr(" This means that you will start with an empty workspace."));
  s.append(tr(" Your old data files have been preserved in $HOME/Qtstalker"));
  s.append(tr(" and can still be accessed by older Qtstalker versions."));
  s.append(tr(" If you don't intend to downgrade to a previous Qtstalker"));
  s.append(tr(" version, then you can remove that directory."));

  Q3TextEdit *message = new Q3TextEdit(w);
  message->setReadOnly(TRUE);
  message->setText(s);
  vbox->addWidget(message);

  check = new QCheckBox(tr("Do not show this message again."), w);
  vbox->addWidget(check);

  addTab(w, tr("Warning"));

  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
}

bool UpgradeMessage::getStatus ()
{
  return check->isChecked();
}

void UpgradeMessage::createPage034 ()
{
  QWidget *w = new QWidget(this);

  Q3VBoxLayout *vbox = new Q3VBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QString s = tr("Your workspace will be converted into the ~/.qtstalker/data1/ directory.");
  s.append(tr(" It could take a long time if there are many data items."));
  s.append(tr(" When satisfied, the old workspace can be manually removed from ~/.qtstalker/data0/\n"));
  s.append(tr("\n"));
  s.append(tr(" If you choose Cancel, then Quit immediately and see the cleanup notes in docs/install.html"));
  Q3TextEdit *message = new Q3TextEdit(w);
  message->setReadOnly(TRUE);
  message->setText(s);
  vbox->addWidget(message);

  Q3HBoxLayout *hbox = new Q3HBoxLayout(vbox);
  hbox->setSpacing(2);

  QLabel *label = new QLabel(tr("Progress"), w);
  hbox->addWidget(label);

  progBar = new Q3ProgressBar(w);
  hbox->addWidget(progBar);

  addTab(w, tr("Chart Conversion"));

  QPushButton *button = new QPushButton(tr("Perform Conversion"), w);
  QObject::connect(button, SIGNAL(clicked()), this, SLOT(convert034()));
  vbox->addWidget(button);

  vbox->addStretch(1);

  setOkButton(QString::null);

  setCancelButton(tr("&Cancel"));

  resize(640, 480);
}

void UpgradeMessage::convert034 ()
{
  // get all chart paths
  QStringList symbolList;
  Config config;
  QString dataPath;
  config.getData(Config::DataPath, dataPath);
  int t = dataPath.find("/data1/", 0, TRUE);
  dataPath.replace(t + 5, 1, "0");
  Traverse trav(Traverse::File);
  trav.traverse(dataPath);
  trav.getList(symbolList);

  QString s;
  config.getData(Config::IndexPath, s);
  index.open(s);

  int loop;
  for (loop = 0; loop < (int) symbolList.count(); loop++)
  {
    progBar->setProgress(loop, (int) symbolList.count());

    if (createDir(symbolList[loop]))
      continue;

    qDebug("Converting %s", symbolList[loop].latin1());
    if (createChart(symbolList[loop]))
      continue;
  }

  index.close();

  copyFiles();

  progBar->setProgress((int) symbolList.count(), (int) symbolList.count());

  qDebug("Conversion complete");
}

bool UpgradeMessage::createChart (QString &path)
{
  DB *olddb = 0;
  int rc = db_create(&olddb, NULL, 0);
  if (rc)
  {
   qDebug("UpgradeMessage::createChart: %s", db_strerror(rc));
    return TRUE;
  }

  rc = olddb->open(olddb, NULL, (char *) path.latin1(), NULL, DB_BTREE, DB_RDONLY, 0664);
  if (rc)
  {
    qDebug("UpgradeMessage::createChart: %s", db_strerror(rc));
    return TRUE;
  }

  QString newPath = path;
  int t = newPath.find("/data0/", 0, TRUE);
  newPath.replace(t + 5, 1, "1");
  DbPlugin db;
  if (db.open(newPath, &index))
  {
    olddb->close(olddb, 0);
    return TRUE;
  }

  QFileInfo fi(newPath);
  QString fn = fi.fileName();

  DBIndexItem item;

  DBT key, data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  olddb->cursor(olddb, NULL, &cur, 0);
  while (! cur->c_get(cur, &key, &data, DB_NEXT))
  {
    QString k = (char *) key.data;
    QString d = (char *) data.data;
    if (key.size != 15)
    {
      saveHeaderData(db, k, d, fn, item);
      continue;
    }

    Bar bar;
    if (bar.setDate(k))
      continue;

    QStringList l = QStringList::split(",", d, FALSE);
    bar.setOpen(l[0].toDouble());
    bar.setHigh(l[1].toDouble());
    bar.setLow(l[2].toDouble());
    bar.setClose(l[3].toDouble());
    bar.setVolume(l[4].toDouble());
    if (l.count() == 6)
      bar.setOI(l[5].toInt());
    db.setBar(bar);
  }
  cur->c_close(cur);

  olddb->close(olddb, 0);
  db.close();

  index.setIndexItem(fn, item);

  return FALSE;
}

void UpgradeMessage::saveHeaderData (DbPlugin &db, QString &k, QString &d, QString &sym, DBIndexItem &item)
{
  // is this a co key?
  bool ok = FALSE;
  double t = k.toDouble(&ok);
  if (ok)
  {
    if (t < 10000)
    {
      // its a chart object
      Setting t;
      t.parse(d);
      QString s = "Plugin";
      QString s2;
      t.getData(s,s2);
      if (s2.length())
      {
        t.remove(s);
        s = "Type";
        t.setData(s, s2);
      }

      s = "Plot";
      t.getData(s, s2);
      if (! s2.compare("Main Plot"))
      {
        s2 = "Bars";
        t.setData(s, s2);
      }
      else
        return;

      index.setChartObject(sym, k, t);
      return;
    }
  }

  if (! k.compare("Type"))
  {
    item.setType(d);
    return;
  }

  if (! k.compare("FuturesType"))
  {
    item.setFuturesType(d);
    return;
  }

  if (! k.compare("FuturesMonth"))
  {
    item.setFuturesMonth(d);
    return;
  }

  if (! k.compare("BarType"))
  {
    item.setBarType((bool)d.toInt());
    return;
  }

  if (! k.compare("Fundamentals"))
  {
    index.setFundamentals(sym, d);
    return;
  }

  if (! k.compare("LocalIndicators"))
  {
    index.addIndicator(sym, d);
    return;
  }

  if (! k.compare("QuotePlugin"))
  {
    item.setQuotePlugin(d);
    return;
  }

  if (! k.compare("Symbol"))
  {
    item.setSymbol(d);
    return;
  }

  if (! k.compare("Title"))
  {
    item.setTitle(d);
    return;
  }

  if (! k.compare("Path"))
  {
    item.setPath(d);
    return;
  }

  if (! k.compare("SpreadFirstSymbol"))
  {
    int t = d.find("/data0/", 0, TRUE);
    d.replace(t + 5, 1, "1");
    QString ts = "FirstSymbol";
    db.setData(ts, d);
    return;
  }

  if (! k.compare("SpreadSecondSymbol"))
  {
    int t = d.find("/data0/", 0, TRUE);
    d.replace(t + 5, 1, "1");
    QString ts = "SecondSymbol";
    db.setData(ts, d);
    return;
  }

  if (! k.compare("IndexList"))
  {
    while (1)
    {
      int t = d.find("/data0/", 0, TRUE);
      if (t == -1)
        break;
      else
        d.replace(t + 5, 1, "1");
    }
    QString ts = "List";
    db.setData(ts, d);
    return;
  }

  if (! k.compare("CCAdjustment"))
  {
    QString ts = "Adjustment";
    db.setData(ts, d);
    return;
  }
}

bool UpgradeMessage::createDir (QString &p)
{
  QString path = p;
  int t = path.find("/data0/", 0, TRUE);
  path.replace(t + 5, 1, "1");

  QStringList l = QStringList::split("/", path, FALSE);
  int loop = l.findIndex(".qtstalker");
  loop = loop + 2;
  for (; loop < (int) l.count() - 1; loop++)
  {
    QString s;
    int loop2;
    for (loop2 = 0; loop2 <= loop; loop2++)
      s.append("/" + l[loop2]);

    QDir dir(s);
    if (! dir.exists(s))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("UpgradeMessage::createDir: error %s", s.latin1());
        return TRUE;
      }
    }
  }

  return FALSE;
}

void UpgradeMessage::copyFiles ()
{
  QString oldPath, newPath;
  Config config;
  config.getData(Config::Home, oldPath);
  newPath = oldPath;
  oldPath.truncate(oldPath.length() - 1);
  oldPath.append("0");
  Traverse trav(Traverse::File);

  // copy group files
  qDebug("Converting group files...");

  QStringList l;
  QString s = oldPath + "/group";
  trav.traverse(s);
  trav.getList(l);

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    s = fi.readLink();
    int t = s.find("/data0/", 0, TRUE);
    s.replace(t + 5, 1, "1");

    if (createDir(l[loop]))
      continue;

    QString s2 = l[loop];
    t = s2.find("/data0/", 0, TRUE);
    s2.replace(t + 5, 1, "1");

    s = "ln -s " + s + " " + s2;

    if (system(s.latin1()))
      qDebug("UpgradeMessage::copyFiles: error creating symlink for group file");
  }

  // copy indicator files
  qDebug("Converting indicator files...");
  s = "cp -R " + oldPath + "/indicator " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying indicator files");
  l.clear();
  s = newPath + "/indicator";
  trav.clear();
  trav.traverse(s);
  trav.getList(l);
  correctPathFiles(l);

  // copy plugin files
  qDebug("Converting plugin files...");
  s = "cp -R " + oldPath + "/plugin " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying indicator files");

  // copy portfolio files
  qDebug("Converting portfolio files...");
  s = "cp -R " + oldPath + "/portfolio " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying portfolio files");
  l.clear();
  s = newPath + "/portfolio";
  trav.clear();
  trav.traverse(s);
  trav.getList(l);
  correctPathFiles(l);

  // copy scanner files
  qDebug("Converting scanner files...");
  s = "cp -R " + oldPath + "/scanner " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying scanner files");
  l.clear();
  s = newPath + "/scanner";
  trav.clear();
  trav.traverse(s);
  trav.getList(l);
  correctPathFiles(l);

  // copy test files
  qDebug("Converting test files...");
  s = "cp -R " + oldPath + "/test " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying test files");
  l.clear();
  s = newPath + "/test";
  trav.clear();
  trav.traverse(s);
  trav.getList(l);
  correctPathFiles(l);
}

void UpgradeMessage::correctPathFiles (QStringList &list)
{
  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile inFile(list[loop]);
    if (! inFile.open(QIODevice::ReadOnly))
    {
      qDebug("UpgradeMessage::correctPathFiles: error opening input file");
      continue;
    }
    Q3TextStream inStream(&inFile);

    QStringList l;
    while(inStream.atEnd() == 0)
    {
      QString s = inStream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;
      l.append(s);
    }
    inFile.close();

    QFile outFile(list[loop]);
    if (! outFile.open(QIODevice::WriteOnly))
    {
      qDebug("UpgradeMessage::correctPathFiles: error opening output file");
      continue;
    }
    Q3TextStream outStream(&outFile);

    int loop2;
    for (loop2 = 0; loop2 < (int) l.count(); loop2++)
    {
      int t = l[loop2].find("/data0/", 0, TRUE);
      if (t != -1)
        l[loop2].replace(t + 5, 1, "1");
      outStream << l[loop2] << "\n";
    }

    outFile.close();
  }
}
