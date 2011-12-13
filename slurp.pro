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
CONFIG += qt debug console #release
QT = core gui network webkit 

# Input
HEADERS += inc/eventer.h inc/parser.h inc/util.h inc/globals.h inc/interacter.h
SOURCES += src/slurper.cpp src/eventer.cpp src/parser.cpp src/util.cpp src/interacter.cpp
