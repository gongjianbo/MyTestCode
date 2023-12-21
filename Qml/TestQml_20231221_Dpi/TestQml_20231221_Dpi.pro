QT += core gui quick widgets
QT += quick-private

CONFIG += c++11 utf8_source

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

HEADERS += \
    mywindow.h \
    mywindow_p.h \
    mywindowattached.h

SOURCES += \
        main.cpp \
        mywindow.cpp \
        mywindowattached.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
