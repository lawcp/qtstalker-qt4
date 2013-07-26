TEMPLATE = lib

!win32 {
	CONFIG += thread warn_on debug
} else {
	CONFIG += thread warn_on debug_and_release
	CONFIG(debug, debug|release): DESTDIR = ../mingwin32/deploydebug
	CONFIG(release, debug|release): DESTDIR = ../mingwin32/deploy
        DEFINES += MINGW
}

CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -ffast-math
#QMAKE CXXFLAGS += -Os

HEADERS += QuotePlugin.h
HEADERS += IndicatorPlugin.h
HEADERS += DbPlugin.h
HEADERS += Setting.h
HEADERS += FuturesData.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += SymbolDialog.h
HEADERS += SymbolButton.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
HEADERS += FileButton.h
HEADERS += Toolbar.h
HEADERS += Bar.h
HEADERS += Config.h
HEADERS += FormulaEdit.h
HEADERS += Indicator.h
HEADERS += COBase.h
HEADERS += HelpWindow.h
HEADERS += Plot.h
HEADERS += DatePlot.h
HEADERS += ScalePlot.h
HEADERS += IndicatorPlot.h
HEADERS += Navigator.h
HEADERS += BarEdit.h
HEADERS += UpgradeMessage.h
HEADERS += BuyArrow.h
HEADERS += Cycle.h
HEADERS += FiboLine.h
HEADERS += HorizontalLine.h
HEADERS += SellArrow.h
HEADERS += Text.h
HEADERS += TrendLine.h
HEADERS += VerticalLine.h
HEADERS += TradeItem.h
HEADERS += StocksDialog.h
HEADERS += FuturesDialog.h
HEADERS += IndexDialog.h
HEADERS += Traverse.h
HEADERS += DataWindow.h
HEADERS += IndicatorSummary.h
HEADERS += XmlWriter.h
HEADERS += DBBase.h
HEADERS += DBIndex.h
HEADERS += DBIndexItem.h
HEADERS += BARS.h
HEADERS += CUS.h
HEADERS += ExScript.h
HEADERS += FI.h
HEADERS += LMS.h
HEADERS += LOWPASS.h
HEADERS += PP.h
HEADERS += SINWAV.h
HEADERS += SYMBOL.h
HEADERS += SZ.h
HEADERS += TALIB.h
HEADERS += THERM.h
HEADERS += UTIL.h
HEADERS += VFI.h
HEADERS += VIDYA.h
HEADERS += VOL.h
HEADERS += qtsFFT.h
HEADERS += CUSDialog.h
HEADERS += Preferences.h
HEADERS += Exchange.h
HEADERS += ToolBarBtn.h
HEADERS += RcFile.h

SOURCES += QuotePlugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += DbPlugin.cpp
SOURCES += Setting.cpp
SOURCES += FuturesData.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += SymbolDialog.cpp
SOURCES += SymbolButton.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
SOURCES += FileButton.cpp
SOURCES += Toolbar.cpp
SOURCES += Bar.cpp
SOURCES += Config.cpp
SOURCES += FormulaEdit.cpp
SOURCES += Indicator.cpp
SOURCES += COBase.cpp
SOURCES += HelpWindow.cpp
SOURCES += Plot.cpp
SOURCES += DatePlot.cpp
SOURCES += ScalePlot.cpp
SOURCES += IndicatorPlot.cpp
SOURCES += Navigator.cpp
SOURCES += BarEdit.cpp
SOURCES += UpgradeMessage.cpp
SOURCES += BuyArrow.cpp
SOURCES += Cycle.cpp
SOURCES += FiboLine.cpp
SOURCES += HorizontalLine.cpp
SOURCES += SellArrow.cpp
SOURCES += Text.cpp
SOURCES += TrendLine.cpp
SOURCES += VerticalLine.cpp
SOURCES += TradeItem.cpp
SOURCES += StocksDialog.cpp
SOURCES += FuturesDialog.cpp
SOURCES += IndexDialog.cpp
SOURCES += Traverse.cpp
SOURCES += DataWindow.cpp
SOURCES += IndicatorSummary.cpp
SOURCES += XmlWriter.cpp
SOURCES += DBBase.cpp
SOURCES += DBIndex.cpp
SOURCES += DBIndexItem.cpp
SOURCES += BARS.cpp
SOURCES += CUS.cpp
SOURCES += ExScript.cpp
SOURCES += FI.cpp
SOURCES += LMS.cpp
SOURCES += LOWPASS.cpp
SOURCES += PP.cpp
SOURCES += SINWAV.cpp
SOURCES += SYMBOL.cpp
SOURCES += SZ.cpp
SOURCES += TALIB.cpp
SOURCES += THERM.cpp
SOURCES += UTIL.cpp
SOURCES += VFI.cpp
SOURCES += VIDYA.cpp
SOURCES += VOL.cpp
SOURCES += qtsFFT.cpp
SOURCES += CUSDialog.cpp
SOURCES += Preferences.cpp
SOURCES += Exchange.cpp
SOURCES += ToolBarBtn.cpp
SOURCES += RcFile.cpp

TARGET = qtstalker

win32 {
	CONFIG(debug, debug|release): TARGET = qtstalkerD
}

VERSION = $${PG_VERSION_PLUGIN}.0

unix {
  # qmake automatically adds /usr/lib
  exists(/usr/local/lib) {
    LIBS += -L/usr/local/lib
  }
  exists(/usr/lib/libdb-4.8.so) {
    LIBS += -ldb-4.8
  } else {
    LIBS += -ldb
  }
}


win32 {
    INCLUDEPATH += ../mingwin32/ta-lib/include
    INCLUDEPATH += ../mingwin32/db/include
    LIBS += -L../mingwin32/ta-lib/lib
    LIBS += -L../mingwin32/db/lib
	LIBS += -lta_lib
	LIBS += -lwsock32
  } else {
  TA_LIB_VERSION = $$system(ta-lib-config --version)
  contains(TA_LIB_VERSION, 0.3.0) {
    LIBS += -lta_abstract
    LIBS += -lta_common
    LIBS += -lta_func
  } else {
    LIBS += -lta_lib
  }
}

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

!win32 {
	target.path = "$${INSTALL_LIB_DIR}"
	INSTALLS += target
}

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level
# qtstalker.pro so doing it here.
QMAKE_DISTCLEAN += qtstalker_defines.h

#The following line was inserted by qt3to4
QT +=  qt3support
