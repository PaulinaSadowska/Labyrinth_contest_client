#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T11:07:04
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    GlobalMapManager.cpp \
    RobotManager.cpp

HEADERS  += mainwindow.h \
    GlobalMapManager.h \
    RobotManager.h

FORMS    += mainwindow.ui
