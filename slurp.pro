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
CONFIG += qt debug console 
QT = core gui network webkit 

# Input
HEADERS += \
    inc/globals.h     \ 
    inc/eventer.h     \
    inc/parser.h      \
    inc/interacter.h  \ 
    inc/about.h       \

SOURCES += \
    src/slurper.cpp    \
    src/eventer.cpp    \
    src/parser.cpp     \
    src/interacter.cpp \ 
    src/about.cpp
