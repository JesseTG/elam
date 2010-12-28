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
	elamunary.h \
	elambinary.h \
	elamcharclass.h \
	elamengine.h \
	elamexpression.h \
	elamvalue.h \
	elamintengine.h \
	elamfloatengine.h \
	elamboolengine.h \
	elamstringengine.h

SOURCES += \
	elamvalue.cpp \
	elamunary.cpp \
	elambinary.cpp \
	elamcharclass.cpp \
	elamengine.cpp \
	elamintengine.cpp \
	elamexpression.cpp \
	elamfloatengine.cpp \
	elamboolengine.cpp \
	elamstringengine.cpp

INCLUDEPATH += .
DEPENDPATH += .
