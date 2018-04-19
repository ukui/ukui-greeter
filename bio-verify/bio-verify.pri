QT += core dbus widgets

greaterThan(QT_MAJOR_VERSION, 4)

HEADERS += \
    $$PWD/biodeviceview.h \
    $$PWD/biocustomtype.h \
    $$PWD/biodevices.h \
    $$PWD/bioauthentication.h

SOURCES += \
    $$PWD/biodeviceview.cpp \
    $$PWD/biocustomtype.cpp \
    $$PWD/biodevices.cpp \
    $$PWD/bioauthentication.cpp

RESOURCES += \
    $$PWD/bioverify.qrc

