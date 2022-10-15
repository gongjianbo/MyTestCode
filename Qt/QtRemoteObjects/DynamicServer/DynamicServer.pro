QT += core gui widgets remoteobjects

CONFIG += c++11 utf8_source

SOURCES += \
    MySource.cpp \
    main.cpp \
    ServerWindow.cpp

HEADERS += \
    MySource.h \
    ServerWindow.h

FORMS += \
    ServerWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
