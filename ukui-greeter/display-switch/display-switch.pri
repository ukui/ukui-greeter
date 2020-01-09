Qt += core gui widgets

DEFINES += QT_DEPRECATED_WARNINGS

FORMS +=

HEADERS += \
    $$PWD/displayservice.h \
    $$PWD/displayswitch.h \
    $$PWD/ui_mainwindow.h

SOURCES += \
    $$PWD/displayservice.cpp \
    $$PWD/displayswitch.cpp

RESOURCES += \
    $$PWD/ds-res.qrc


