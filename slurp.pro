# QT Variables
TEMPLATE = app
TARGET = slurp
DEPENDPATH += . inc src
INCLUDEPATH += . inc
DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = obj/moc
RCC_DIR = obj/rcc
UI_DIR = obj/ui

# Link in the required libraries
CONFIG += qt link_pkgconfig debug#release
QT = core gui network webkit 
PKGCONFIG = libevent libevent_pthreads libcurl

# Input
HEADERS += inc/eventer.h inc/threader.h inc/retriever.h inc/parser.h inc/callbacks.h inc/util.h inc/globals.h
SOURCES += src/slurper.cpp src/threader.cpp src/eventer.cpp src/retriever.cpp src/parser.cpp src/callbacks.cpp src/util.cpp
