TEMPLATE = app
TARGET = elamdemo
CONFIG += link_prl
LIBS += -L.. -lelam

INCLUDEPATH += ../src

SOURCES += demo.cpp
HEADERS += demo.h