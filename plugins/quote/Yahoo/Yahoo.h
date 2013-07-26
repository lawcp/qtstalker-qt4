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

#include "QuotePlugin.h"
#include <qstring.h>
//Added by qt3to4:
#include <Q3PtrList>
#include "Setting.h"
#include "DbPlugin.h"
#include "Config.h"
#include <q3listbox.h>
#include <q3datetimeedit.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include "FileButton.h"

class Yahoo : public QuotePlugin
{
  Q_OBJECT

  public:
    Yahoo ();
    virtual ~Yahoo ();
    void update ();
    QString parseDate (QString &);
    void parseHistory ();
    void parseQuote ();
    void loadSettings ();
    void saveSettings ();
    void printErrorList ();
    void cancelUpdate ();
    void parseFundamental ();
    void loadAllSymbols ();
    void createHistoryUrls (QString &);
    void createAutoHistoryUrls (QString &, QString &);
    void createQuoteUrls (QString &);
    void createFundamentalUrls (QString &);
    void buildGui ();
    void setMethod (QString);
    void getExchange (QString &, QString &);

  public slots:
    void startDownload ();
    void fileDone (bool);
    void timeoutError ();
    void newStock ();
    void methodChanged (int);
    void allSymbolsChecked (bool);

  private:
    QStringList symbolList;
    Q3PtrList<Setting> url;
    Setting *currentUrl;
    QString dataPath;
    QStringList fileList;
    QStringList errorList;
    QStringList monthList;
    DbPlugin plug;
    Config config;
    FileButton *list;
    QCheckBox *adjustment;
    Q3DateEdit *sdate;
    Q3DateEdit *edate;
    QComboBox *method;
    QCheckBox *allSymbols;
    int fileCount;
    int fileCounter;
};

extern "C"
{
  QuotePlugin * createQuotePlugin ();
}

