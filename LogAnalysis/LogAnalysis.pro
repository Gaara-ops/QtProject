#-------------------------------------------------
#
# Project created by QtCreator 2018-11-16T17:25:27
#
#-------------------------------------------------
include($$PWD/../Globe/Globe.pri)
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LogAnalysis
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
