#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T09:53:53
#
#-------------------------------------------------

QT  += core gui widgets dbus x11extras

greaterThan(QT_MAJOR_VERSION, 4)

TARGET = ukui-greeter
TEMPLATE = app

PREFIX = /usr/share/ukui-greeter

DEFINES += QT_MESSAGELOGCONTEXT \    #使用qInstallMessageHandler定制日志输出格式
           GREETER_CONFIG=/usr/share/lightdm/ukui-greeter/ \
           RESOURCE_PATH=$${PREFIX} \
           QT_DISABLE_DEPRECATED_BEFORE=0

SOURCES += \
    greeter/greeterwindow.cpp \
    greeter/iconedit.cpp \
    greeter/loginwindow.cpp \
    greeter/main.cpp \
    greeter/pagelistview.cpp \
    greeter/userentry.cpp \
    greeter/usersmodel.cpp \
    greeter/userwindow.cpp \
    greeter/greeterwrapper.cpp \
    greeter/globalv.cpp \
    greeter/screenmodel.cpp \
    greeter/mainwindow.cpp \
    greeter/sessionwindow.cpp \
    greeter/powerwindow.cpp \
    greeter/proxymodel.cpp

HEADERS  += \
    greeter/greeterwindow.h \
    greeter/iconedit.h \
    greeter/loginwindow.h \
    greeter/pagelistview.h \
    greeter/userentry.h \
    greeter/usersmodel.h \
    greeter/userwindow.h \
    greeter/greeterwrapper.h \
    greeter/globalv.h \
    greeter/screenmodel.h \
    greeter/mainwindow.h \
    greeter/sessionwindow.h \
    greeter/powerwindow.h \
    greeter/proxymodel.h

CONFIG += c++11 debug

INCLUDEPATH += /usr/include/lightdm-qt5-3

LIBS += /usr/lib/x86_64-linux-gnu/liblightdm-qt5-3.so \
        /usr/lib/x86_64-linux-gnu/libX11.so.6 \
        /usr/lib/x86_64-linux-gnu/libXrandr.so

RESOURCES += image.qrc

TRANSLATIONS += translations/zh_CN.ts

system(lrelease translations/zh_CN.ts -qm translations/zh_CN.qm)

qm_file.files = translations/*.qm
qm_file.path = $${PREFIX}/translations/

resourcefiles.files = resource/*
resourcefiles.path = $${PREFIX}/images/

configfile.files = 95-ukui-greeter.conf
configfile.path = /usr/share/lightdm/lightdm.conf.d/

desktopfile.files = ukui-greeter.desktop
desktopfile.path = /usr/share/xgreeters/

manfile.files = man/ukui-greeter.1.gz
manfile.path = /usr/share/man/man1/

target.path = /usr/sbin/

INSTALLS += target configfile desktopfile resourcefiles qm_file manfile



