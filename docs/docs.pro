TEMPLATE = lib

# install the css, html the js files
docs.path = "$${INSTALL_DOCS_DIR}/qtstalker/html"
docs.files = *.css
docs.files += *.html
docs.files += *.js
INSTALLS += docs

# install the image/png files
pics.path = "$${INSTALL_DOCS_DIR}/qtstalker/html"
pics.files = *.png
INSTALLS += pics

# install the example indicator html files and pics
idocs.path = "$${INSTALL_DOCS_DIR}/qtstalker/html/indicator"
idocs.files = indicator/*.html
idocs.files += indicator/*.png
INSTALLS += idocs

# install the CHANGELOG files
cl.path = "$${INSTALL_DOCS_DIR}/qtstalker/html"
cl.files = CHANGELOG*
INSTALLS += cl

# install the past CHANGELOG files
pcl.path = "$${INSTALL_DOCS_DIR}/qtstalker/html/pastchanges"
pcl.files = pastchanges/*.html
pcl.files += pastchanges/CHANGELOG*
INSTALLS += pcl

# install the translation files
i18n.path = "$${INSTALL_DATA_DIR}/qtstalker/i18n"
i18n.files = ../i18n/*.qm
INSTALLS += i18n

# install the indicator files
indicator.path = "$${INSTALL_DATA_DIR}/qtstalker/indicator"
# only install some explicit examples
indicator.files = ../misc/CUS_examples/AROON
indicator.files += ../misc/CUS_examples/AROONOSC
indicator.files += ../misc/CUS_examples/ATRP
indicator.files += ../misc/CUS_examples/DMI
indicator.files += ../misc/CUS_examples/MACD
indicator.files += ../misc/CUS_examples/RSI
indicator.files += ../misc/CUS_examples/STOCH
indicator.files += ../misc/CUS_examples/VOL
indicator.files += ../misc/CUS_examples/VOLR
indicator.files += ../misc/CUS_examples/WILL-R
indicator.files += ../misc/CUS_examples/cdl-hac-ma
indicator.files += ../misc/CUS_examples/cdl-rel
indicator.files += ../misc/CUS_examples/cdl-rel-bb
indicator.files += ../misc/CUS_examples/cdl-rel-bo
indicator.files += ../misc/CUS_examples/cdl-rel-dc
indicator.files += ../misc/CUS_examples/cdl-rel-ma
indicator.files += ../misc/CUS_examples/cdl-rel-pp
indicator.files += ../misc/CUS_examples/ohlc-rel
indicator.files += ../misc/CUS_examples/ohlc-rel-bb
indicator.files += ../misc/CUS_examples/ohlc-rel-bo
indicator.files += ../misc/CUS_examples/ohlc-rel-dc
indicator.files += ../misc/CUS_examples/ohlc-rel-ma
indicator.files += ../misc/CUS_examples/ohlc-rel-pp
INSTALLS += indicator

# extra source files
DISTFILES += AUTHORS
DISTFILES += BUGS
DISTFILES += CHANGELOG-0.37
DISTFILES += COPYING
DISTFILES += INSTALL
DISTFILES += TODO
DISTFILES += add-nav-av.list
DISTFILES += add-nav-av.pl
DISTFILES += add-nav.list
DISTFILES += add-nav.pl

#The following line was inserted by qt3to4
QT +=  qt3support
