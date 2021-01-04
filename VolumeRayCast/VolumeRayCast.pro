#-------------------------------------------------
#
# Project created by QtCreator 2019-08-28T10:33:40
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
TARGET = VolumeRayCast
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    datamanager.cpp \
    renderingmanager.cpp \
    renderingparameters.cpp \
    volrenderbase.cpp \
    vr1pass.cpp \
    file/pvm.cpp \
    file/rawloader.cpp \
    glutils/arrayobject.cpp \
    glutils/bufferobject.cpp \
    glutils/computeshader.cpp \
    glutils/framebufferobject.cpp \
    glutils/gutils.cpp \
    glutils/shader.cpp \
    glutils/texture1d.cpp \
    glutils/texture2d.cpp \
    glutils/texture3d.cpp \
    math/simplearoundcamera.cpp \
    volrend/reader.cpp \
    volrend/transferfunction.cpp \
    volrend/transferfunction1d.cpp \
    volrend/utils.cpp \
    volrend/volume.cpp


FreeGlutDir=F:/opengl/freeglut-3.0.0/qt5.3/lib-release
GlewDir = F:/opengl/glew-2.1.0/qt5.3/lib-release
GLMDir = F:/opengl/glm-0.9.9.4
STDDir = F:/vulkan/stb

INCLUDEPATH += $${FreeGlutDir}/include
INCLUDEPATH += $${GlewDir}/include
INCLUDEPATH += $${GLMDir}/
INCLUDEPATH += $${STDDir}/

LIBS += -L$${FreeGlutDir}/lib libfreeglut
LIBS += -L$${GlewDir}/lib libglew32
LIBS += -lopengl32 -lwinmm -lgdi32 -lm -lglu32

HEADERS += \
    datamanager.h \
    renderingmanager.h \
    renderingparameters.h \
    volrenderbase.h \
    vr1pass.h \
    file/pvm.h \
    file/rawloader.h \
    glutils/arrayobject.h \
    glutils/bufferobject.h \
    glutils/computeshader.h \
    glutils/framebufferobject.h \
    glutils/gutils.h \
    glutils/shader.h \
    glutils/texture1d.h \
    glutils/texture2d.h \
    glutils/texture3d.h \
    math/defines.h \
    math/simplearoundcamera.h \
    volrend/reader.h \
    volrend/transferfunction.h \
    volrend/transferfunction1d.h \
    volrend/utils.h \
    volrend/volume.h

