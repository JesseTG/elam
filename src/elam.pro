TEMPLATE = lib
TARGET = elam
DESTDIR = ..
CONFIG += staticlib create_prl
QT -= gui
OBJECTS_DIR = .ctmp
MOC_DIR = .ctmp
RCC_DIR = .ctmp


HEADERS += \
	elam.h \
	elamengine.h \
	elamexpression.h \
	elamvalue.h

SOURCES += \
	elamvalue.cpp \
	elamengine.cpp

INCLUDEPATH += .
DEPENDPATH += .
