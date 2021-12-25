QT += core
QT -= gui

CONFIG += c++11 console utf8_source
CONFIG -= app_bundle

SOURCES += \
    main.cpp

HEADERS += \
    ByteArray.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
