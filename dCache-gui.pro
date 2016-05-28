#-------------------------------------------------
#
# Project created by QtCreator 2016-05-22T17:54:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dCache-gui
TEMPLATE = app

SOURCES += src\dCache-gui.cpp\
        src\Logger.cpp\
        src\dCacheMainWindow.cpp \
        src\dCacheTools.cpp

HEADERS  += include\dCacheMainWindow.h\
            include\Logger.h \
            include\dCacheTools.h

FORMS    += ui\dCacheMainWindow.ui

INCLUDEPATH += include
