#-------------------------------------------------
#
# Project created by QtCreator 2016-04-02T23:00:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Kakuro
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    kkrboard.cpp \
    problemdata.cpp \
    playstatus.cpp \
    useranswer.cpp \
    useranswermanager.cpp \
    inputdrag.cpp \
    inputfactory.cpp

HEADERS  += mainwindow.h \
    kkrboard.h \
    problemdata.h \
    playstatus.h \
    useranswer.h \
    useranswermanager.h \
    cosmetic.h \
    inputfactory.h \
    inputdrag.h

CONFIG += c++11
