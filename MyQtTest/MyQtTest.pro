#-------------------------------------------------
#
# Project created by QtCreator 2017-10-17T18:50:15
#
#-------------------------------------------------
include($$PWD/../Globe/Globe.pri)

QT       += core gui multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyQtTest

TEMPLATE = app

CONFIG += c++11 qt warn_on

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
         += globeinclude.h \

FORMS    += mainwindow.ui


#添加log4cpp头文件和库
INCLUDEPATH += F:\log4cpp\log4cpp-lib\include
LIBS += F:/log4cpp/log4cpp-lib/lib/liblog4cppD.a


LIBS += -lopengl32 -lwinmm -lgdi32 -lm -lglu32 -lpsapi
