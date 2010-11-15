TEMPLATE = app
TARGET = parsertest
QT -= gui
CONFIG += qtestlib debug link_prl
INCLUDEPATH += . ../../src
DEPENDPATH += $$INCLUDEPATH ../..
LIBS += -L../.. -lelam

SOURCES += parser.cpp
HEADERS += parser.h