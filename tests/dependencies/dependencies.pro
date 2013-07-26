!exists(../../.qmake.cache) {
  error("Please do 'configure' for Qtstalker before running tests")
}

QT -= gui

CONFIG += qtestlib
CONFIG -= app_bundle

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

unix {
  exists(/usr/lib/libdb-4.8.so) {
    LIBS += -ldb-4.8
  } else {
    LIBS += -ldb
  }
}

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

# Input
SOURCES += Dependencies.cpp
