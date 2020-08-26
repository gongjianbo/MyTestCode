QT       += core gui widgets

TARGET = SortHeaderView
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11 utf8_source

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    SortHeaderView.cpp

HEADERS += \
        mainwindow.h \
    SortHeaderView.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
