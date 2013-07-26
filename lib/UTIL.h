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

#include "IndicatorPlugin.h"
//Added by qt3to4:
#include <Q3PtrList>

class UTIL : public IndicatorPlugin
{
  public:
    UTIL ();
    virtual ~UTIL ();
    PlotLine * calculateCustom (QString &, Q3PtrList<PlotLine> &);
    PlotLine * calculateADMS (QString &p, Q3PtrList<PlotLine> &d, int);
    PlotLine * calculateAccum (QString &, Q3PtrList<PlotLine> &);
    PlotLine * calculateCOLOR (QString &p, Q3PtrList<PlotLine> &d);
    PlotLine * calculateCOMP (QString &p, Q3PtrList<PlotLine> &d);
    PlotLine * calculateCOUNTER (QString &p, Q3PtrList<PlotLine> &d);
    PlotLine * calculateHL (QString &p, Q3PtrList<PlotLine> &d, int);
    PlotLine * calculateINRANGE (QString &p, Q3PtrList<PlotLine> &d);
    PlotLine * calculateNormal(QString &, Q3PtrList<PlotLine> &);
    PlotLine * calculatePER (QString &p, Q3PtrList<PlotLine> &d);
    PlotLine * calculateREF (QString &p, Q3PtrList<PlotLine> &d);
    PlotLine * calculateSIGN (QString &p, Q3PtrList<PlotLine> &d);
    void formatDialog (QStringList &vl, QString &rv, QString &rs);

  private:
    QStringList methodList;
};
