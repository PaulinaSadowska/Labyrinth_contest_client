#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T11:07:04
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++14


QMAKE_CXXFLAGS += -std=c++14
macx{
    contains(QMAKE_CXX, /usr/bin/clang++)
    {
    message(Using LLVM libc++)
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -lc++
    QMAKE_CC=gcc
    QMAKE_CXX=g++
    }
}

SOURCES += main.cpp\
        mainwindow.cpp \
    RobotManager.cpp \
    MapManager.cpp

HEADERS  += mainwindow.h \
    RobotManager.h \
    MapManager.h

FORMS    += mainwindow.ui
