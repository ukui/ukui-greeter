#-------------------------------------------------
#
# Project created by QtCreator 2018-01-23T15:09:47
#
#-------------------------------------------------

QT       += core gui x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kylin-greeter-rootimage
TEMPLATE = app


SOURCES +=\
    rootimageapp.cpp

HEADERS  += \
    rootimageapp.h

FORMS    +=

LIBS += /usr/lib/x86_64-linux-gnu/libX11.so.6 \
        /usr/lib/x86_64-linux-gnu/libXpm.so.4
target.path = /usr/sbin/
INSTALLS += target
