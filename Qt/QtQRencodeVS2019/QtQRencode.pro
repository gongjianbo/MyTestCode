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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libqrencode/lib/ -lqrencode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libqrencode/lib/ -lqrencoded

INCLUDEPATH += $$PWD/libqrencode/include
DEPENDPATH += $$PWD/libqrencode/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

