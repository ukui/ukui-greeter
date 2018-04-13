QT += core dbus widgets

greaterThan(QT_MAJOR_VERSION, 4)

HEADERS += \
    $$PWD/biodeviceview.h \
    $$PWD/bioauthenticationview.h \
    $$PWD/biocustomtype.h

SOURCES += \
    $$PWD/biodeviceview.cpp \
    $$PWD/bioauthenticationview.cpp \
    $$PWD/biocustomtype.cpp

RESOURCES += \
    $$PWD/bioverify.qrc

