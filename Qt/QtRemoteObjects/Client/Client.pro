QT += core gui widgets remoteobjects

CONFIG += c++11 utf8_source

#replica to client
REPC_REPLICA += $$PWD/../Rep/simple.rep
INCLUDEPATH += $$PWD/../Rep

SOURCES += \
    main.cpp \
    ClientWindow.cpp

HEADERS += \
    ClientWindow.h \
    $$PWD/../Rep/Define.h

FORMS += \
    ClientWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
