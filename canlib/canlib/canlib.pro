#-------------------------------------------------
#
# Project created by QtCreator 2017-05-22T16:02:54
#
#-------------------------------------------------

QT      += core serialbus
QT      -= gui

TARGET = canlib

TEMPLATE = lib
#CONFIG += staticlib

#чтобы не добавлялся номер версии к имени файла
#VERSION = 1.0.0.1
#TARGET_EXT = .so

#QMAKE_TARGET = canlib.so

DEFINES += CANLIB_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QMAKE_LFLAGS += -static -static-libgcc

SOURCES += canlib.cpp

HEADERS += canlib.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#LIBS += -Wl, -Bstatic, -lboost_regex, -lgmp, -lgmpxx, -Bdynamic

#DISTFILES += \
#    canlib.def

