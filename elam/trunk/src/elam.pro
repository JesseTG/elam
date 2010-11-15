TEMPLATE = lib
TARGET = elam
DESTDIR = ..
CONFIG += staticlib create_prl
QT -= gui
OBJECTS_DIR = .ctmp
MOC_DIR = .ctmp
RCC_DIR = .ctmp


HEADERS += \
	dptr.h \
	elam.h \
	elamengine.h \
	elamexpression.h \
	elamvalue.h \
	elamintengine.h \
	elamfloatengine.h

SOURCES += \
	elamvalue.cpp \
	elamengine.cpp \
	elamintengine.cpp \
	elamexpression.cpp \
	elamfloatengine.cpp

INCLUDEPATH += .
DEPENDPATH += .
