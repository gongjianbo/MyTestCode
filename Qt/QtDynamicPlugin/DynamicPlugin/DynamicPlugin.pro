QT += core gui widgets

TEMPLATE = lib
CONFIG += c++17 utf8_source plugin

DESTDIR = $$PWD/../bin/myplugins

INCLUDEPATH += $$PWD/../Interface
include($$PWD/../Interface/Interface.pri)

HEADERS += \
    DynamicWidget.h

SOURCES += \
    DynamicWidget.cpp

OTHER_FILES += dynamicwidget.json

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

