#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T09:53:53
#
#-------------------------------------------------

QT  += core gui widgets dbus x11extras svg

greaterThan(QT_MAJOR_VERSION, 4)

TARGET = ukui-greeter
TEMPLATE = app

PREFIX = /usr/share/ukui-greeter

DEFINES += QT_MESSAGELOGCONTEXT \    #使用qInstallMessageHandler定制日志输出格式
           GREETER_CONFIG=/usr/share/lightdm/ukui-greeter/ \
           RESOURCE_PATH=$${PREFIX} \
           QT_DISABLE_DEPRECATED_BEFORE=0

include(display-switch/display-switch.pri)
include(common/common.pri)
include(VirtualKeyboard/VirtualKeyboard.pri)
include(BiometricAuth/BiometricAuth.pri)

INCLUDEPATH += \
            common \
            VirtualKeyboard/src \
            BiometricAuth

SOURCES += \
    greeter/greeterwindow.cpp \
    greeter/iconedit.cpp \
    greeter/loginwindow.cpp \
    greeter/main.cpp \
    greeter/usersmodel.cpp \
    greeter/greeterwrapper.cpp \
    greeter/screenmodel.cpp \
    greeter/mainwindow.cpp \
    greeter/sessionwindow.cpp \
    greeter/powerwindow.cpp \
    greeter/proxymodel.cpp \
    greeter/userentry.cpp \
    greeter/usersview.cpp \
    greeter/language.cpp \
    greeter/languagewidget.cpp \
    greeter/rootWindowBackground.cpp

HEADERS  += \
    greeter/greeterwindow.h \
    greeter/iconedit.h \
    greeter/loginwindow.h \
    greeter/usersmodel.h \
    greeter/greeterwrapper.h \
    greeter/screenmodel.h \
    greeter/mainwindow.h \
    greeter/sessionwindow.h \
    greeter/powerwindow.h \
    greeter/proxymodel.h \
    greeter/userentry.h \
    greeter/usersview.h \
    greeter/language.h \
    greeter/languagewidget.h \
    greeter/rootWindowBackground.h

CONFIG += c++11 debug link_pkgconfig

QMAKE_CXXFLAGS += -Wdeprecated-declarations

PKGCONFIG += liblightdm-qt5-3 x11 xrandr xtst imlib2

RESOURCES += image.qrc \
    qss.qrc

TRANSLATIONS += translations/zh_CN.ts \
                translations/ru.ts \
                translations/fr.ts \
                translations/pt.ts \
                translations/es.ts

system("lrelease translations/*.ts")

qm_file.files = translations/*.qm
qm_file.path = $${PREFIX}/translations/

resourcefiles.files = resource/*
resourcefiles.path = $${PREFIX}/images/

badgefiles.files = resource/badges/*.svg
badgefiles.path = $${PREFIX}/images/badges/

configfile.files = 95-ukui-greeter.conf
configfile.path = /usr/share/lightdm/lightdm.conf.d/

etcfile.files = ukui-greeter.conf
etcfile.path = /etc/lightdm/

desktopfile.files = ukui-greeter.desktop
desktopfile.path = /usr/share/xgreeters/

manfile.files = man/ukui-greeter.1.gz
manfile.path = /usr/share/man/man1/

target.path = /usr/sbin/

INSTALLS += target configfile etcfile desktopfile resourcefiles qm_file manfile badgefiles
