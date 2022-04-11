TEMPLATE     = lib
CONFIG      += plugin
INCLUDEPATH += .
INCLUDEPATH += ../../../../ ../../../../src/
TARGET       = gambit_fileformat_fasta
DESTDIR      = ../../../../../plugins
HEADERS     += GFastaReader.h \
               GFastaFormatManager.h
SOURCES     += GFastaReader.cpp \
               GFastaFormatManager.cpp
