QT += core gui widgets network

CONFIG += c++11 utf8_source

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LocalDefine.cpp \
    LocalHost.cpp \
    LocalNode.cpp \
    VoicePrintClient.cpp \
    VoicePrintSource.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    LocalDefine.h \
    LocalHost.h \
    LocalNode.h \
    VoicePrintClient.h \
    VoicePrintSource.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
