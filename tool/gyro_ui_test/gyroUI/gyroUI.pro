#-------------------------------------------------
#
# Project created by QtCreator 2016-12-13T15:41:45
#
#-------------------------------------------------

QT       += core gui opengl
LIBS     += -lglut -lGLU

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gyroUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp

HEADERS  += mainwindow.h \
    glwidget.h

FORMS    += mainwindow.ui
