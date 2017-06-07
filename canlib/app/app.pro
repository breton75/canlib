#-------------------------------------------------
#
# Project created by QtCreator 2017-04-26T12:36:57
#
#-------------------------------------------------

QT       += core gui serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ../canlib/canlib.h

FORMS    += mainwindow.ui

#QMAKE_LFLAGS += -static



RESOURCES += \
    res.qrc


#unix:!macx|win32: LIBS += -L$$PWD/../canlib_build_static/release/ -lcanlib

#INCLUDEPATH += $$PWD/../canlib
#DEPENDPATH += $$PWD/../canlib

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../canlib_build_static/release/canlib.lib
#else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/../canlib_build_static/release/libcanlib.a


unix:!macx|win32: LIBS += -L$$PWD/../canlib_build_static/ -lcanlib
INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../
PRE_TARGETDEPS += $$PWD/../canlib_build_static/libcanlib.a
