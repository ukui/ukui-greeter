Qt += core gui widgets

DEFINES += QT_DEPRECATED_WARNINGS

FORMS +=

HEADERS += \
    $$PWD/displayservice.h \
    $$PWD/displayswitch.h \
    $$PWD/mainwindow.h \
    $$PWD/ui_mainwindow.h \
    $$PWD/keyeventmonitor.h

SOURCES += \
    $$PWD/displayservice.cpp \
    $$PWD/displayswitch.cpp \
    $$PWD/keyeventmonitor.cpp

RESOURCES += \
    $$PWD/ds-res.qrc


