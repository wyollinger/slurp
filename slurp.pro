# QT Variables
TEMPLATE = app
VERSION = 0.0.3
TARGET = slurp
DEPENDPATH += . inc src
INCLUDEPATH += . inc
DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = obj/moc
RCC_DIR = obj/rcc
UI_DIR = obj/ui

# Tell QT to use flex 
include(flex.pri)

QMAKE_LEX = flex
FLEXSOURCES = flex/scanner.fl

# Link in the required libraries
CONFIG += qt link_pkgconfig 
QT = core gui network thread
PKGCONFIG = libevent libevent_pthreads libcurl

# Input
HEADERS += inc/eventer.h inc/retriever.h inc/scanner.h
SOURCES += src/slurper.cpp src/eventer.cpp src/retriever.cpp 
