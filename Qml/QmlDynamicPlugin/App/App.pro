QT += quick

CONFIG += c++17 utf8_source

DESTDIR = $$PWD/../bin

INCLUDEPATH += $$PWD/../Interface
include($$PWD/../Interface/Interface.pri)

SOURCES += \
        main.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
