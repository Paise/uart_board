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
    customstatusbar.cpp

HEADERS  += mainwindow.h \
    configuredialog.h \
    customstatusbar.h

FORMS    += mainwindow.ui \
    configuredialog.ui \
    about.ui
