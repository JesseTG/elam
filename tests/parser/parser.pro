TEMPLATE = app
TARGET = parsertest
QT -= gui
CONFIG += qtestlib debug
INCLUDEPATH += . ../../src
DEPENDPATH += $$INCLUDEPATH
LIBS += -L../.. -lelam

SOURCES += parser.cpp
HEADERS += parser.h