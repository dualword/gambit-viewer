TEMPLATE     = lib
CONFIG      += plugin
INCLUDEPATH += .
INCLUDEPATH += ../../../../ ../../../../src/
TARGET       = gambit_fileformat_vcf
DESTDIR      = ../../../../../plugins
SOURCES     += GVcfReader.cpp \
               GVcfFormatManager.cpp
HEADERS     += GVcfReader.h \
               GVcfFormatManager.h
