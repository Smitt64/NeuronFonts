#-------------------------------------------------
#
# Project created by QtCreator 2015-07-10T09:19:06
#
#-------------------------------------------------

QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): {
    QT += widgets

    win32: {
        qtHaveModule(winextras): {
            QT += winextras
            DEFINES += WIN_DWM
        }
    }
}

TARGET = NeuronFonts
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    neuron.cpp

HEADERS  += mainwindow.h \
    neuron.h

FORMS    += mainwindow.ui

UI_DIR = .build
MOC_DIR = .build
RCC_DIR = .build
OBJECTS_DIR = .build
