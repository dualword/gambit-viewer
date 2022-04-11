TEMPLATE     = lib
CONFIG      += plugin
INCLUDEPATH += .
INCLUDEPATH += ../../../../ ../../../../src/
TARGET       = gambit_fileformat_gff3
DESTDIR      = ../../../../../plugins
SOURCES     += GGff3Reader.cpp \
               GGff3FormatManager.cpp
HEADERS     += GGff3Reader.h \
               GGff3FormatManager.h
