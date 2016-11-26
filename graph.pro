QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -std=c++11 -g -pthread -Iinc
QMAKE_LFLAGS += -lglut -lGL -lGLU -lfreeimage

TARGET = 3dviewer
TEMPLATE = app

HEADERS += \
    inc/glut_thread.h \
    inc/loadobj.h \
    inc/mainwindow.h \
    inc/model.h \
    inc/objshape.h \
    inc/shape.h \
    inc/ui_mainwindow.h

SOURCES += \
    src/glut_thread.cpp \
    src/loadobj.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/model.cpp \
    src/objshape.cpp \
    src/shape.cpp

FORMS += \
    ui/mainwindow.ui

