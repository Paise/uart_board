#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T11:09:19
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uart_board
TEMPLATE = app


SOURCES += \
    src/configuredialog.cpp \
    src/customstatusbar.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/messagelogger.cpp \
    src/rawdatasendwidget.cpp \
    src/serialportholder.cpp

HEADERS  += \
    src/configuredialog.h \
    src/customstatusbar.h \
    src/iserialio.h \
    src/mainwindow.h \
    src/messagelogger.h \
    src/rawdatasendwidget.h \
    src/serialportholder.h

FORMS    += \
    ui/about.ui \
    ui/configuredialog.ui \
    ui/mainwindow.ui \
    ui/rawdatasendwidget.ui

RESOURCES +=

include(ext/QtAwesome/QtAwesome.pri)

#Target version
VERSION = 0.0.1
DEFINES += APPLICATION_VERSION=\\\"$$VERSION\\\"
DEFINES += ORGANIZATION_NAME=\\\"SPbSPU\\\"
DEFINES += APPLICATION_NAME=\\\"UartBoard\\\"
