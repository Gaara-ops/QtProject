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

VTK_DIR = F:/VTK/VTK-7.1/qt5.3/lib-debug
INCLUDEPATH += $${VTK_DIR}/include/vtk-7.1

LIBS += -lgomp
LIBS += -L$${VTK_DIR}/lib \
        libvtkCommonCore-7.1 \
        libvtkIOImage-7.1 \
        libvtkCommonDataModel-7.1 \
        libvtkCommonExecutionModel-7.1 \

FreeGlutDir=F:/opengl/freeglut-3.0.0/qt5.3/lib-debug
GlewDir = F:/opengl/glew-2.1.0/qt5.3/lib-debug
GLMDir = F:/opengl/glm-0.9.9.4

INCLUDEPATH += $${FreeGlutDir}/include
INCLUDEPATH += $${GlewDir}/include
INCLUDEPATH += $${GLMDir}/

LIBS += -L$${FreeGlutDir}/lib libfreeglut
LIBS += -L$${GlewDir}/lib libglew32d

LIBS += -lopengl32 -lwinmm -lgdi32 -lm -lglu32
