#-------------------------------------------------
#
# Project created by QtCreator 2015-07-10T09:19:06
#
#-------------------------------------------------

QT += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets winextras

TARGET = NeuronFonts
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    neuron.cpp

HEADERS  += mainwindow.h \
    neuron.h

FORMS    += mainwindow.ui
