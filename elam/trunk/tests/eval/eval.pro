TEMPLATE = app
TARGET = evaltest
QT -= gui
CONFIG += qtestlib debug link_prl
INCLUDEPATH += . ../../src
DEPENDPATH += $$INCLUDEPATH ../..
LIBS += -L../.. -lelam

SOURCES += eval.cpp
HEADERS += eval.h