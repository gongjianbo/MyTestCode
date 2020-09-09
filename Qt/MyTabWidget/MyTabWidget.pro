QT  += core gui widgets

CONFIG += c++11 utf8_source
TARGET = MyTabWidget
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    MyTabPage.cpp \
    MyTabWidget.cpp \
    MyTabBar.cpp \
    MyTabPopup.cpp

HEADERS += \
        mainwindow.h \
    MyTabPage.h \
    MyTabWidget.h \
    MyTabBar.h \
    MyTabPopup.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
