#-------------------------------------------------
#
# Project created by QtCreator 2018-09-27T17:50:33
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RayTraceWidget
TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp

SOURCES += main.cpp\
        mainwindow.cpp \
    raytraceinfo.cpp \
    showimage.cpp

HEADERS  += mainwindow.h \
    myhead.h \
    mymath.h \
    raytraceinfo.h \
    showimage.h

FORMS    += mainwindow.ui

VTK_DIR = F:/VTK-7.1/vtk-libdebug
INCLUDEPATH += $${VTK_DIR}/include/vtk-7.1

LIBS += -lgomp
LIBS += -L$${VTK_DIR}/lib \
        libvtkCommonCore-7.1 \
        libvtkIOImage-7.1 \
        libvtkCommonDataModel-7.1 \
        libvtkCommonExecutionModel-7.1 \
