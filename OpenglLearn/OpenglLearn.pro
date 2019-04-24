#-------------------------------------------------
#
# Project created by QtCreator 2019-04-23T11:10:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = OpenglLearn
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    math3d.cpp \
    pipeline.cpp

freeglutDir_D = F:/opengl/freeglut-3.0.0/qt5.3/lib-debug
freeglutDir   = F:/opengl/freeglut-3.0.0/qt5.3/lib-release
glewDir_D     = F:/opengl/glew-2.1.0/qt5.3/lib-debug
glewDir       = F:/opengl/glew-2.1.0/qt5.3/lib-release

glmDir    = F:/opengl/glm-0.9.9.4/glm
ogldevDir = F:/opengl/ogldev-source/Include

INCLUDEPATH += $${glmDir}/
INCLUDEPATH += $${ogldevDir}/

LIBS += -lgomp
LIBS += -lopengl32 -lwinmm -lgdi32 -lm -lglu32 -lpsapi

CONFIG(debug):{
INCLUDEPATH += $${freeglutDir_D}/include
INCLUDEPATH += $${glewDir_D}/include
LIBS += -L$${freeglutDir_D}/lib \
        libfreeglut \

LIBS += -L$${glewDir_D}/lib \
        libglew32d
}
else:CONFIG(release):{
INCLUDEPATH += $${freeglutDir}/include
INCLUDEPATH += $${glewDir}/include
LIBS += -L$${freeglutDir}/lib \
        libfreeglut \

LIBS += -L$${glewDir}/lib \
        libglew32
}

OTHER_FILES += \
    readme.txt \
    shader.vs \
    shader.fs

HEADERS += \
    math3d.h \
    opengl_util.h \
    pipeline.h


