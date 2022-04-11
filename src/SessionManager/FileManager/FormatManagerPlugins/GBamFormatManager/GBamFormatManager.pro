TEMPLATE     = lib
CONFIG      += plugin
INCLUDEPATH += .
INCLUDEPATH += ../../../../ ../../../../src/
TARGET       = gambit_fileformat_bam
DESTDIR      = ../../../../../plugins

# Use native zlib on non-Windows platforms
!win32 { 
    LIBS += -lz
    exists ( ./zlib.h ):system(rm ./zlib.h)
    exists ( ./zconf.h ):system(rm ./zconf.h)
}
SOURCES += GBamReader.cpp \
    GBamFormatManager.cpp \
    BamReader.cpp \
    BGZF.cpp
HEADERS += GBamReader.h \
    GBamFormatManager.h \
    BamReader.h \
    BamAux.h \
    BGZF.h

# Add included zlib headers for Windows platforms
win32:HEADERS += ./zconf.h \
    ./zlib.h
