QT += core gui widgets

CONFIG += c++11 utf8_source
DESTDIR = $$PWD/../bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    ServerWindow.cpp

HEADERS += \
    ServerWindow.h

FORMS += \
    ServerWindow.ui

INCLUDEPATH += $$PWD/../Common
DEPENDPATH += $$PWD/../Common
LIBS += $$PWD/../lib/Common.lib
PRE_TARGETDEPS += $$PWD/../lib/Common.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
