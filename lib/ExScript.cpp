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

#include "ExScript.h"
#include "PrefDialog.h"
#ifdef MINGW
//gje - had to include this. see line 154
#include <QWaitCondition>
#include <QMutex>
#endif

#include <q3dict.h>
#include <q3cstring.h>
#include <qmessagebox.h>
#include <unistd.h>
//Added by qt3to4:
#include <Q3PtrList>

ExScript::ExScript ()
{
  pluginName = "ExScript";
  helpFile = "exscript.html";
  colorLabel = "color";
  labelLabel = "label";
  lineTypeLabel = "lineType";
  scriptPathLabel = "scriptPath";
  comlineParmsLabel = "comlineParms";
  pluginLabel = "plugin";
  dateLabel = "dateCheck";
  openLabel = "openCheck";
  highLabel = "highCheck";
  lowLabel = "lowCheck";
  closeLabel = "closeCheck";
  volumeLabel = "volumeCheck";
  oiLabel = "oiCheck";
  timeoutLabel = "timeout";

  formatList.append(FormatString);
  formatList.append(FormatString);
  formatList.append(FormatBool);
  formatList.append(FormatBool);
  formatList.append(FormatBool);
  formatList.append(FormatBool);
  formatList.append(FormatBool);
  formatList.append(FormatBool);
  formatList.append(FormatBool);

  proc = 0;

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerDone()));

  setDefaults();
}

ExScript::~ExScript ()
{
  if (proc)
    delete proc;

  delete timer;
}

void ExScript::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  dateFlag = FALSE;
  openFlag = FALSE;
  highFlag = FALSE;
  lowFlag = FALSE;
  closeFlag = FALSE;
  volumeFlag = FALSE;
  oiFlag = FALSE;
  seconds = 10;
}

Indicator * ExScript::calculate ()
{
  Indicator *output = new Indicator;
  output->setDateFlag(dateFlag);
  output->setLogScale(logScale);

  output->addLine(doScript());

  return output;
}

PlotLine * ExScript::doScript ()
{
  if (proc)
  {
    delete proc;
    proc = 0;
  }

  PlotLine *line = new PlotLine();

  if (! scriptPath.length())
  {
    qDebug("ExScript::calculate: no script path");
    return line;
  }

  proc = new Q3Process(this);
  connect(proc, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()));
  proc->setCommunication(Q3Process::Stdin|Q3Process::Stdout|Q3Process::Stderr);
  proc->addArgument(scriptPath);

  QStringList l = QStringList::split(" ", comlineParms, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    proc->addArgument(l[loop]);

  buffer.truncate(0);

  QString s;
  if (dateFlag || openFlag || highFlag || lowFlag || closeFlag || volumeFlag || oiFlag)
    getInput(s);

  QByteArray ba(s.length());
  if (s.length())
  {
    for (loop = 0; loop < (int) s.length(); loop++)
      ba[loop] = s.at(loop).latin1();
  }

  if (! proc->launch(ba, NULL))
  {
    qDebug("ExScript::calculate: error starting script");
    delete proc;
    proc = 0;
    return line;
  }

  timer->start(seconds * 1000, FALSE);

  wakeup();

  while (proc->isRunning())
  {
  #ifdef MINGW
    //gjn - does not compile on windows (no usleep)
    //so replace it with more qt-like stuff. this is kind
    //of ugly right now. exposing qthread::msleep could be better option
    QWaitCondition w;
    QMutex sleepmutex;
    sleepmutex.lock();
    w.wait(&sleepmutex, 100);
    sleepmutex.unlock();
    //gje - end of patch
#else
    usleep(100);
#endif
    wakeup();
  }

  timer->stop();

  if (proc)
  {
    delete proc;
    proc = 0;
  }

  if (! buffer.length())
  {
    qDebug("ExScript::createOutput: output buffer empty");
    return line;
  }

  l = QStringList::split(",", buffer, FALSE);
  for (loop = 0; loop < (int) l.count(); loop++)
    line->append(l[loop].toDouble());

  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  return line;
}

void ExScript::getInput (QString &ss)
{
  int loop;
  QString s;
  for (loop = 0; loop < data->count(); loop++)
  {
    Bar bar;
    data->getBar(loop, bar);

    if (dateFlag)
    {
      bar.getDateTimeString(FALSE, s);
      ss.append(s + ",");
    }

    if (openFlag)
      ss.append(QString::number(bar.getOpen()) + ",");

    if (highFlag)
      ss.append(QString::number(bar.getHigh()) + ",");

    if (lowFlag)
      ss.append(QString::number(bar.getLow()) + ",");

    if (closeFlag)
      ss.append(QString::number(bar.getClose()) + ",");

    if (volumeFlag)
      ss.append(QString::number(bar.getVolume(), 'f', 0) + ",");

    if (oiFlag)
      ss.append(QString::number(bar.getOI()) + ",");

    ss.truncate(ss.length() - 1);
    ss.append("\n");
  }
}

void ExScript::readFromStdout ()
{
  QByteArray ba = proc->readStdout();
  buffer.append(ba);
}

int ExScript::indicatorPrefDialog (QWidget *w)
{
  QString pl2 = QObject::tr("Output");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString spl = QObject::tr("Script Path");
  QString clsl = QObject::tr("Switches");
  QString pl = QObject::tr("Input");
  QString dl = QObject::tr("Date");
  QString ol = QObject::tr("Open");
  QString hl = QObject::tr("High");
  QString lol = QObject::tr("Low");
  QString cll = QObject::tr("Close");
  QString vl = QObject::tr("Volume");
  QString oil = QObject::tr("Open Interest");
  QString tol = QObject::tr("Timeout Seconds");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("ExScript Indicator"));
  dialog->setHelpFile(helpFile);

  dialog->createPage (pl);
  QStringList l;
  l.append(scriptPath);
  dialog->addFileItem(spl, pl, l, scriptPath);
  dialog->addTextItem(clsl, pl, comlineParms);
  dialog->addIntItem(tol, pl, seconds, 1, 999);
  dialog->addCheckItem(dl, pl, dateFlag);
  dialog->addCheckItem(ol, pl, openFlag);
  dialog->addCheckItem(hl, pl, highFlag);
  dialog->addCheckItem(lol, pl, lowFlag);
  dialog->addCheckItem(cll, pl, closeFlag);
  dialog->addCheckItem(vl, pl, volumeFlag);
  dialog->addCheckItem(oil, pl, oiFlag);

  dialog->createPage(pl2);
  dialog->addColorItem(cl, pl2, color);
  dialog->addTextItem(ll, pl2, label);
  dialog->addComboItem(ltl, pl2, lineTypes, lineType);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getFile(spl, l);
    if (l.count())
      scriptPath = l[0];
    dialog->getText(clsl, comlineParms);
    seconds = dialog->getInt(tol);
    dateFlag = dialog->getCheck(dl);
    openFlag = dialog->getCheck(ol);
    highFlag = dialog->getCheck(hl);
    lowFlag = dialog->getCheck(lol);
    closeFlag = dialog->getCheck(cll);
    volumeFlag = dialog->getCheck(vl);
    oiFlag = dialog->getCheck(oil);

    dialog->getColor(cl, color);
    dialog->getText(ll, label);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    rc = TRUE;
  }
  else
    rc = FALSE;

  delete dialog;
  return rc;
}

void ExScript::setIndicatorSettings (Setting &dict)
{
  setDefaults();

  if (! dict.count())
    return;

  QString s;
  dict.getData(colorLabel, s);
  if (s.length())
    color.setNamedColor(s);

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(scriptPathLabel, s);
  if (s.length())
    scriptPath = s;

  dict.getData(comlineParmsLabel, s);
  if (s.length())
    comlineParms = s;

  dict.getData(dateLabel, s);
  if (s.length())
    dateFlag = s.toInt();

  dict.getData(openLabel, s);
  if (s.length())
    openFlag = s.toInt();

  dict.getData(highLabel, s);
  if (s.length())
    highFlag = s.toInt();

  dict.getData(lowLabel, s);
  if (s.length())
    lowFlag = s.toInt();

  dict.getData(closeLabel, s);
  if (s.length())
    closeFlag = s.toInt();

  dict.getData(volumeLabel, s);
  if (s.length())
    volumeFlag = s.toInt();

  dict.getData(oiLabel, s);
  if (s.length())
    oiFlag = s.toInt();

  dict.getData(timeoutLabel, s);
  if (s.length())
    seconds = s.toInt();
}

void ExScript::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(scriptPathLabel, scriptPath);
  dict.setData(comlineParmsLabel, comlineParms);
  ts = QString::number(dateFlag);
  dict.setData(dateLabel, ts);
  ts = QString::number(openFlag);
  dict.setData(openLabel, ts);
  ts = QString::number(highFlag);
  dict.setData(highLabel, ts);
  ts = QString::number(lowFlag);
  dict.setData(lowLabel, ts);
  ts = QString::number(closeFlag);
  dict.setData(closeLabel, ts);
  ts = QString::number(volumeFlag);
  dict.setData(volumeLabel, ts);
  ts = QString::number(oiFlag);
  dict.setData(oiLabel, ts);
  ts = QString::number(seconds);
  dict.setData(timeoutLabel, ts);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * ExScript::calculateCustom (QString &p, Q3PtrList<PlotLine> &d)
{
  // format1: SCRIPT_PATH, COMMAND_LINE_SWITCHES, DATE, OPEN, HIGH, LOW, CLOSE, VOLUME, OI

  if (checkFormat(p, d, 9, 9))
    return 0;

  scriptPath = formatStringList[0];
  comlineParms = formatStringList[1];

  if (! formatStringList[2].compare("TRUE"))
    dateFlag = TRUE;
  else
    dateFlag = FALSE;

  if (! formatStringList[3].compare("TRUE"))
    openFlag = TRUE;
  else
    openFlag = FALSE;

  if (! formatStringList[4].compare("TRUE"))
    highFlag = TRUE;
  else
    highFlag = FALSE;

  if (! formatStringList[5].compare("TRUE"))
    lowFlag = TRUE;
  else
    lowFlag = FALSE;

  if (! formatStringList[6].compare("TRUE"))
    closeFlag = TRUE;
  else
    closeFlag = FALSE;

  if (! formatStringList[7].compare("TRUE"))
    volumeFlag = TRUE;
  else
    volumeFlag = FALSE;

  if (! formatStringList[8].compare("TRUE"))
    oiFlag = TRUE;
  else
    oiFlag = FALSE;

  return doScript();
}

void ExScript::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString spl = QObject::tr("Script Path");
  QString clsl = QObject::tr("Switches");
  QString dl = QObject::tr("Date");
  QString ol = QObject::tr("Open");
  QString hl = QObject::tr("High");
  QString lol = QObject::tr("Low");
  QString cll = QObject::tr("Close");
  QString vl = QObject::tr("Volume");
  QString oil = QObject::tr("Open Interest");

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("ExScript Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);
  QStringList l;
  dialog->addFileItem(spl, pl, l, scriptPath);
  dialog->addTextItem(clsl, pl, comlineParms);
  dialog->addCheckItem(dl, pl, dateFlag);
  dialog->addCheckItem(ol, pl, openFlag);
  dialog->addCheckItem(hl, pl, highFlag);
  dialog->addCheckItem(lol, pl, lowFlag);
  dialog->addCheckItem(cll, pl, closeFlag);
  dialog->addCheckItem(vl, pl, volumeFlag);
  dialog->addCheckItem(oil, pl, oiFlag);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    dialog->getFile(spl, l);
    if (l.count())
      rs = l[0];
    dialog->getText(clsl, comlineParms);
    rs.append("," + comlineParms);

    bool t = dialog->getCheck(dl);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");

    t = dialog->getCheck(ol);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");

    t = dialog->getCheck(hl);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");

    t = dialog->getCheck(lol);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");

    t = dialog->getCheck(cll);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");

    t = dialog->getCheck(vl);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");

    t = dialog->getCheck(oil);
    if (t)
      rs.append(",TRUE");
    else
      rs.append(",FALSE");
  }

  delete dialog;
}

void ExScript::timerDone ()
{
  if (! proc->isRunning())
    return;

  int rc = QMessageBox::warning(0,
    			        tr("ExScript Warning"),
			        tr("Script timeout. Cancel process?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
  {
    timer->start(seconds * 1000, FALSE);
    return;
  }

  proc->kill();
  delete proc;
  proc = 0;
}
