QT += qt3support

exists(.qmake.cache) {
  message("Using .qmake.cache")
}
else {
  message("Doing automated configuration ...")

  unix {
    MY_OS += "unix"
    exists(/usr/local/include/ta-lib) {
      INCLUDEPATH += /usr/local/include/ta-lib
    }
    exists(/usr/include/ta-lib) {
      INCLUDEPATH += /usr/include/ta-lib
    }
  }

  freebsd-g++ {
    MY_OS += "freebsd"
    exists(/usr/local/include/db48) {
      INCLUDEPATH += /usr/local/include/db43
    }
    exists(/usr/local/lib/db48) {
      LIBS += -L/usr/local/lib/db43
    }
    INCLUDEPATH += /usr/X11R6/include
  }

  linux-g++ {
    MY_OS += "linux"
  }

  win32 {
    MY_OS += "windows"
  }

  macx {
    MY_OS += "macx"
    # For MacPorts
    exists(/opt/local/include/db4) {
    }
    exists(/opt/local/include/ta-lib) {
      INCLUDEPATH += /opt/local/include/ta-lib
    }
    exists(/opt/local/lib) {
      LIBS += -L/opt/local/lib
    }
    # For Fink
    exists(/sw/include/db4) {
      INCLUDEPATH += /sw/include/db4
    }
    exists(/sw/include/ta-lib) {
      INCLUDEPATH += /sw/include/ta-lib
    }
    exists(/sw/lib) {
      LIBS += -L/sw/lib
    }
  }

  message("Using INCLUDEPATH=$$INCLUDEPATH")
  message("Using LIBS=$$LIBS")

  !exists(qtstalker.cfg) {
	error("Missing configuration file qtstalker.cfg")
  }
	include(qtstalker.cfg)

  INCLUDEPATH = $$unique(INCLUDEPATH)
  LIBS = $$unique(LIBS)

  message("Writing .qmake.cache")
  system(echo "INCLUDEPATH=$$INCLUDEPATH" > .qmake.cache)
  system(echo "LIBS=$$LIBS" >> .qmake.cache)
  system(echo "MY_OS=$$MY_OS" >> .qmake.cache)
  system(echo "INSTALL_BIN_DIR=$$INSTALL_BIN_DIR" >> .qmake.cache)
  system(echo "INSTALL_DATA_DIR=$$INSTALL_DATA_DIR" >> .qmake.cache)
  system(echo "INSTALL_DOCS_DIR=$$INSTALL_DOCS_DIR" >> .qmake.cache)
  system(echo "INSTALL_LIB_DIR=$$INSTALL_LIB_DIR" >> .qmake.cache)
  system(echo "PG_VERSION_PLUGIN=$$PG_VERSION_PLUGIN" >> .qmake.cache)
  system(echo "PG_VERSION_STRING=$$PG_VERSION_STRING" >> .qmake.cache)

  message("Writing lib/qtstalker_defines.h")
  win32 {
    system(echo $${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H > lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define QTSTALKER_DEFINES_H >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_BIN_DIR \"$$INSTALL_BIN_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_DATA_DIR \"$$INSTALL_DATA_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_DOCS_DIR \"$$INSTALL_DOCS_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_LIB_DIR \"$$INSTALL_LIB_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define PG_VERSION_PLUGIN \"$$PG_VERSION_PLUGIN\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define PG_VERSION_STRING \"$$PG_VERSION_STRING\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}endif >> lib/qtstalker_defines.h)
  } else {
    system(echo \"$${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H\" > lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define QTSTALKER_DEFINES_H\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_BIN_DIR \\\"$$INSTALL_BIN_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_DATA_DIR \\\"$$INSTALL_DATA_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_DOCS_DIR \\\"$$INSTALL_DOCS_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_LIB_DIR \\\"$$INSTALL_LIB_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define PG_VERSION_PLUGIN \\\"$$PG_VERSION_PLUGIN\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define PG_VERSION_STRING \\\"$$PG_VERSION_STRING\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}endif\" >> lib/qtstalker_defines.h)
  }
}
message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")
message("Operating system: $$MY_OS")
!isEmpty(QMAKESPEC) {
  message("QMAKESPEC: $$QMAKESPEC")
}

TEMPLATE = subdirs

# compile TALIB
#SUBDIRS += TALIB

# compile qtstalker lib
SUBDIRS += lib

# compile app
SUBDIRS += src

# compile quote plugins
SUBDIRS += plugins/quote/CME
SUBDIRS += plugins/quote/CSV
SUBDIRS += plugins/quote/NYBOT
SUBDIRS += plugins/quote/Yahoo

# install docs and i18n
SUBDIRS += docs

# extra source files
DISTFILES += README.docs
DISTFILES += README.mingw
DISTFILES += STATUS
DISTFILES += TRANSLATE.HOWTO
DISTFILES += plugin.cfg
DISTFILES += porting-qt4-log.txt.gz
DISTFILES += porting-qt4-notes.txt
DISTFILES += qtstalker.pro.user

