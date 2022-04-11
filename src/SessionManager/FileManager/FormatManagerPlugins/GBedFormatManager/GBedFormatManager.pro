TEMPLATE     = lib
CONFIG      += plugin
INCLUDEPATH += .
INCLUDEPATH += ../../../../ ../../../../src/
TARGET       = gambit_fileformat_bed
DESTDIR      = ../../../../../plugins
SOURCES     += GBedReader.cpp \
               GBedFormatManager.cpp
HEADERS     += GBedReader.h \
               GBedFormatManager.h
