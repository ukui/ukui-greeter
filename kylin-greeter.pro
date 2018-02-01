#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T09:53:53
#
#-------------------------------------------------

QT  += core gui widgets


TARGET = kylin-greeter
TEMPLATE = app

DEFINES += QT_MESSAGELOGCONTEXT \    #使用qInstallMessageHandler定制日志输出格式
           GREETER_CONFIG=/usr/share/lightdm/kylin-greeter

SOURCES += \
    src/extrarowproxymodel.cpp \
    src/greeterwindow.cpp \
    src/iconedit.cpp \
    src/loginwindow.cpp \
    src/main.cpp \
    src/pagelistview.cpp \
    src/userentry.cpp \
    src/usersmodel.cpp \
    src/userwindow.cpp \
    src/greeterwrapper.cpp \
    src/globalv.cpp \
    src/screenmodel.cpp \
    src/mainwindow.cpp \
    src/sessionwindow.cpp \
    src/powerwindow.cpp \
    src/exponentialblur.cpp

HEADERS  += \
    src/extrarowproxymodel.h \
    src/greeterwindow.h \
    src/iconedit.h \
    src/loginwindow.h \
    src/pagelistview.h \
    src/userentry.h \
    src/usersmodel.h \
    src/userwindow.h \
    src/greeterwrapper.h \
    src/globalv.h \
    src/screenmodel.h \
    src/mainwindow.h \
    src/sessionwindow.h \
    src/powerwindow.h \
    src/exponentialblur.h

#SUBDIRS += rootimage
#rootimage.subdir = src
#rootimage.file = root-image.pro
#rootimage.target = root-image

CONFIG += c++11

INCLUDEPATH += /usr/include/lightdm-qt5-3

LIBS += /usr/lib/x86_64-linux-gnu/liblightdm-qt5-3.so \
        /usr/lib/x86_64-linux-gnu/libX11.so.6

RESOURCES += src/resource.qrc \
             src/translate.qrc
TRANSLATIONS += src/ts/zh_CN.ts

#MOC_DIR += build/
#RCC_DIR += build/
#OBJECTS_DIR += build/
#DESTDIR += build/

configfile.path = /usr/share/lightdm/lightdm.conf.d/
configfile.files = debian/95-kylin-greeter.conf

desktopfile.path = /usr/share/xgreeters/
desktopfile.files = debian/kylin-greeter.desktop

target.path += /usr/bin
INSTALLS += target configfile desktopfile


DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

#TODO: 1.对屏幕大小变化的处理（针对虚拟机）
#      2.对自动登录的处理
#      3.语言选择
##      4.电源对话框
#      5.对焦点的处理（启动时用户头像没有边框）
#      6.登录后的屏幕图片
#      7.远程登录的支持
