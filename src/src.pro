TEMPLATE = app

!win32 {
	CONFIG += qt thread warn_on debug
} else {
	CONFIG += thread warn_on debug_and_release
	CONFIG(debug, debug|release): DESTDIR = ../mingwin32/deploydebug
	CONFIG(release, debug|release): DESTDIR = ../mingwin32/deploy
        DEFINES += MINGW
}

CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math
#QMAKE_CXXFLAGS += -Os

HEADERS += Qtstalker.h
HEADERS += PortfolioDialog.h
HEADERS += Tester.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += PortfolioPage.h
HEADERS += TestPage.h
HEADERS += IndicatorPage.h
HEADERS += NavigatorTab.h
HEADERS += ScannerPage.h
HEADERS += Scanner.h
HEADERS += ChartToolbar.h
HEADERS += ExtraToolbar.h
HEADERS += MainMenubar.h
HEADERS += TesterReport.h
HEADERS += TesterRulePage.h
HEADERS += TesterTestPage.h
HEADERS += TesterStopPage.h
HEADERS += TesterChartPage.h


SOURCES += Qtstalker.cpp
SOURCES += PortfolioDialog.cpp
SOURCES += Tester.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += PortfolioPage.cpp
SOURCES += TestPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += NavigatorTab.cpp
SOURCES += ScannerPage.cpp
SOURCES += Scanner.cpp
SOURCES += ChartToolbar.cpp
SOURCES += ExtraToolbar.cpp
SOURCES += MainMenubar.cpp
SOURCES += TesterReport.cpp
SOURCES += TesterRulePage.cpp
SOURCES += TesterTestPage.cpp
SOURCES += TesterStopPage.cpp
SOURCES += TesterChartPage.cpp


TARGET = qtstalker

win32 {
	CONFIG(debug, debug|release): TARGET = qtstalkerD
}

INCLUDEPATH += ../lib

win32 {
  exists(../qtc-gdbmacros/gdbmacros.cpp) {
      CONFIG(debug, debug|release): SOURCES += ../qtc-gdbmacros/gdbmacros.cpp
  }
  INCLUDEPATH += ../mingwin32/ta-lib/include
  INCLUDEPATH += ../mingwin32/db/include
  CONFIG(debug, debug|release): LIBS += -L../mingwin32/deploydebug -lqtstalkerD0
  CONFIG(release, debug|release): LIBS += -L../mingwin32/deploy -lqtstalker0
} else {
  LIBS += -L../lib -lqtstalker
}

unix {
  exists(/usr/lib/libdb-4.8.so) {
    LIBS += -ldb-4.8
  } else {
    LIBS += -ldb
  }
}

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

!win32 {
	target.path = "$${INSTALL_BIN_DIR}"
	INSTALLS += target
}

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level
# qtstalker.pro so doing it here.
QMAKE_DISTCLEAN += ../.qmake.cache

#The following line was inserted by qt3to4
QT +=  qt3support
