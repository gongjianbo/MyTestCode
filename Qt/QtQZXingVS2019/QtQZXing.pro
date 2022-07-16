QT += core gui widgets

CONFIG += c++11 utf8_source

DESTDIR = $$PWD/bin

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QZXing/lib/ -lQZXing3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QZXing/lib/ -lQZXingd3

INCLUDEPATH += $$PWD/QZXing/include
DEPENDPATH += $$PWD/QZXing/include
DEFINES += ENABLE_ENCODER_GENERIC

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
