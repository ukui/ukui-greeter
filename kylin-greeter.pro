#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T09:53:53
#
#-------------------------------------------------

QT       += core gui

QT += widgets

TARGET = kylin-greeter-qt
TEMPLATE = app


SOURCES += \
    src/extrarowproxymodel.cpp \
    src/greeterwindow.cpp \
    src/iconedit.cpp \
    src/loginwindow.cpp \
    src/main.cpp \
    src/pagelistview.cpp \
    src/userentry.cpp \
    src/usersmodel.cpp \
    src/userwindow.cpp

HEADERS  += \
    src/extrarowproxymodel.h \
    src/greeterwindow.h \
    src/iconedit.h \
    src/loginwindow.h \
    src/pagelistview.h \
    src/userentry.h \
    src/usersmodel.h \
    src/userwindow.h

CONFIG += c++11
QMAKE_CXXFLAGS = -g

INCLUDEPATH += /usr/include/lightdm-qt5-3

LIBS += /usr/lib/x86_64-linux-gnu/liblightdm-qt5-3.so

RESOURCES += src/resource.qrc

INSTALLS += target
target.path += /usr/bin

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
