#-------------------------------------------------
#
# Project created by QtCreator 2018-08-31T13:46:49
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyRayTracingLearn
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    boundingbox.cpp \
    entity.cpp \
    light.cpp

HEADERS  += mainwindow.h \
    vec3.h \
    util.h \
    boundingbox.h \
    entity.h \
    light.h

FORMS    += mainwindow.ui
