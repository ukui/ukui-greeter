#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T09:53:53
#
#-------------------------------------------------

QT  += core gui widgets svg dbus x11extras


TARGET = kylin-greeter
TEMPLATE = app

DEFINES += QT_MESSAGELOGCONTEXT \    #使用qInstallMessageHandler定制日志输出格式
           GREETER_CONFIG=/usr/share/lightdm/kylin-greeter/ \
           RESOURCE=/usr/share/kylin-greeter/ \
           USE_QRC=true \

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

CONFIG += c++11 debug

INCLUDEPATH += /usr/include/lightdm-qt5-3

LIBS += /usr/lib/x86_64-linux-gnu/liblightdm-qt5-3.so \
        /usr/lib/x86_64-linux-gnu/libX11.so.6

debug{
    RESOURCES += src/resource.qrc \
                 src/translate.qrc
}
TRANSLATIONS += src/ts/zh_CN.ts


configfile.path = /usr/share/lightdm/lightdm.conf.d/
configfile.files = debian/95-kylin-greeter.conf

desktopfile.path = /usr/share/xgreeters/
desktopfile.files = debian/kylin-greeter.desktop

resourcefiles.path = /usr/share/kylin-greeter/
resourcefiles.files = src/resource/*

target.path += /usr/bin
INSTALLS += target configfile desktopfile resourcefiles


DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

#TODO:
#      1.对屏幕大小变化的处理（针对虚拟机）
##      2.对自动登录的处理
##      3.语言选择
##      4.电源对话框
##      5.对焦点的处理（启动时用户头像没有边框）
#      6.登录后的屏幕图片
#      7.远程登录的支持
##      8.打开电源对话框后再打开语言选择菜单，无法选择菜单
