#-------------------------------------------------
#
# Project created by QtCreator 2013-04-10T20:48:51
#
#-------------------------------------------------

QT       += core gui
unix:QT  += dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tracer
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    skype.cpp

HEADERS  += mainwindow.h \
    skype.h

FORMS    += mainwindow.ui

win32 {
    SOURCES += skype_win.cpp
    HEADERS += skype_win.h
}

unix {
    SOURCES += skype_linux.cpp
    HEADERS += skype_linux.h
}
