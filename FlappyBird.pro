#-------------------------------------------------
#
# Project created by QtCreator 2014-06-14T13:22:29
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlappyBird
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    roaditem.cpp \
    bird.cpp \
    gamecontroller.cpp \
    barrier.cpp

HEADERS  += mainwindow.h \
    roaditem.h \
    bird.h \
    gamecontroller.h \
    define.h \
    barrier.h

RESOURCES += \
    resource.qrc

OTHER_FILES +=
