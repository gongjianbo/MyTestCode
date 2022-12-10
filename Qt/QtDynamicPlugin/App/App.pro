QT += core gui widgets

CONFIG += c++17 utf8_source

DESTDIR = $$PWD/../bin

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

INCLUDEPATH += $$PWD/../Interface
include($$PWD/../Interface/Interface.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
