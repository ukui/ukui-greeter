#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T09:53:53
#
#-------------------------------------------------

QT       += core gui

QT += widgets

TARGET = kylin-greeter
TEMPLATE = app


SOURCES += main.cpp\
        greeterwindow.cpp \
    userwindow.cpp \
    loginwindow.cpp \
    iconedit.cpp \
    tipedit.cpp \
    extrarowproxymodel.cpp \
    usersmodel.cpp \
    userentry.cpp \
    pagelistview.cpp

HEADERS  += greeterwindow.h \
    userwindow.h \
    loginwindow.h \
    iconedit.h \
    ui_login.h \
    tipedit.h \
    extrarowproxymodel.h \
    usersmodel.h \
    userentry.h \
    pagelistview.h \

CONFIG += c++11
QMAKE_CXXFLAGS = -g

INCLUDEPATH += /usr/include/lightdm-qt5-3

LIBS += /usr/lib/x86_64-linux-gnu/liblightdm-qt5-3.so

RESOURCES += \
    resource.qrc

FORMS += \
    login.ui \
    entry.ui \
    userwindow.ui

INSTALLS += target
target.path += /usr/sbin

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
