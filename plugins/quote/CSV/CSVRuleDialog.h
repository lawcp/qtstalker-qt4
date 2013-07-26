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

#include <q3tabdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <q3datetimeedit.h>
#include <qstringlist.h>
#include <q3listbox.h>
#include <qspinbox.h>
#include "FileButton.h"
#include "Toolbar.h"

class CSVRuleDialog : public Q3TabDialog
{
  Q_OBJECT

  public:
    CSVRuleDialog (QWidget *, QString &);
    ~CSVRuleDialog ();
    void createRulePage ();
    void loadRule ();

  public slots:
    void saveRule ();
    void insertField ();
    void deleteField ();
    void help ();
    void comboChanged (int);
    void textChanged (const QString &);
    void ruleFieldSelected (int);
    void fieldListSelected (int);

  private:
    Toolbar *ruleToolbar;
    Q3ListBox *ruleList;
    Q3ListBox *fieldList;
    QLineEdit *ruleName;
    QString helpFile;
    QLineEdit *directory;
    QLineEdit *symbolFilter;
    QComboBox *type;
    QComboBox *delimiter;
    bool saveFlag;
    QString rulePath;
};

