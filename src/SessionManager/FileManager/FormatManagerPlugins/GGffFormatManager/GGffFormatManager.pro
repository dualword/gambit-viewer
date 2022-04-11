TEMPLATE     = lib
CONFIG      += plugin
INCLUDEPATH += .
INCLUDEPATH += ../../../../ ../../../../src/
TARGET       = gambit_fileformat_gff
DESTDIR      = ../../../../../plugins
SOURCES     += GGffReader.cpp \
               GGffFormatManager.cpp
HEADERS     += GGffReader.h \
               GGffFormatManager.h
