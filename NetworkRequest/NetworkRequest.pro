#-------------------------------------------------
#
# Project created by QtCreator 2019-02-12T16:05:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetworkRequest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    networkope.cpp \
    udpserver.cpp \
    udpclient.cpp

HEADERS  += mainwindow.h \
    networkope.h \
    udpserver.h \
    udpclient.h

FORMS    += mainwindow.ui \
    udpserver.ui \
    udpclient.ui
