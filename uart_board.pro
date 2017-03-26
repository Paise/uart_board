#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T11:09:19
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uart_board
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configuredialog.cpp \
    customstatusbar.cpp \
    messagelogger.cpp \
    rawdatasendwidget.cpp \
    serialportholder.cpp

HEADERS  += mainwindow.h \
    configuredialog.h \
    customstatusbar.h \
    messagelogger.h \
    rawdatasendwidget.h \
    serialportholder.h \
    iserialio.h

FORMS    += mainwindow.ui \
    configuredialog.ui \
    about.ui \
    rawdatasendwidget.ui

RESOURCES +=

include(ext/QtAwesome/QtAwesome.pri)

#Target version
VERSION = 0.0.1
DEFINES += APPLICATION_VERSION=\\\"$$VERSION\\\"
DEFINES += ORGANIZATION_NAME=\\\"SPbSPU\\\"
DEFINES += APPLICATION_NAME=\\\"UartBoard\\\"
