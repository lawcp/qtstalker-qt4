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

#ifndef QUOTEPLUGIN_HPP
#define QUOTEPLUGIN_HPP

#include <qstring.h>
#include <q3networkprotocol.h>
#include <q3urloperator.h>
#include <qtimer.h>
#include <q3tabdialog.h>
#include <q3textedit.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <q3progressbar.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include "Toolbar.h"
#include "DBIndex.h"

/**
* \todo
* Needs to be re-written to not use the QUrlOperator.
* See http://doc.trolltech.com/4.3/porting4-overview.html#url-operations-qurloperator
*/
class QuotePlugin : public Q3TabDialog
{
  Q_OBJECT

  signals:
    void signalGetFileDone(bool);
    void signalCopyFileDone(QString);
    void signalTimeout();
    void signalProgMessage(int, int);
    void chartUpdated();
    void signalWakeup();

  public:
    QuotePlugin();
    virtual ~QuotePlugin();
    bool setTFloat(QString &, bool);
    QString substituteSeparator(const QString &, const QChar &, const QChar &);
    void createDirectory(QString &, QString &);
    void getPluginName(QString &);
    void getHelpFile(QString &);
    void buildGui();
    void enableGUI();
    void disableGUI();
    virtual void update();
    void setChartIndex(DBIndex *);
    Q3ProgressBar *progressBar;

  public slots:
    void getFile(QString &);
    void copyFile(QString &, QString &);
    void getFileDone(Q3NetworkOperation *);
    void copyFileDone(Q3NetworkOperation *);
    void dataReady(const QByteArray &, Q3NetworkOperation *);
    void slotTimeout();
    void getQuotes();
    void downloadComplete();
    void cancelDownload();
    void printStatusLogMessage(QString &);
    void help();
    void slotWakeup();

  protected:
    QString file;
    float tfloat;
    bool saveFlag;
    QString pluginName;
    QString helpFile;
    Q3UrlOperator *op;
    QString data;
    QTimer *timer;
    int errorLoop;
    QString stringDone;
    QString stringCanceled;
    Q3TextEdit *statusLog;
    Toolbar *toolbar;
    Q3VBoxLayout *vbox;
    QWidget *baseWidget;
    Q3GridLayout *grid;
    QSpinBox *retrySpin;
    QSpinBox *timeoutSpin;
    DBIndex *chartIndex;
};

#endif
