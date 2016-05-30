#-------------------------------------------------
#
# Project created by QtCreator 2016-05-22T17:54:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dCache-gui
TEMPLATE = app

SOURCES += \
    src/dCache-gui.cpp \
    src/dCacheMainWindow.cpp \
    src/dCacheTools.cpp \
    src/Logger.cpp

HEADERS += \
    include/dCacheMainWindow.h \
    include/dCacheTools.h \
    include/Logger.h

FORMS += \
    ui/dCacheMainWindow.ui

INCLUDEPATH += include
