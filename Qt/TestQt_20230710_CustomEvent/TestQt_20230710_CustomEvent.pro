QT += core
QT += core-private
QT += gui
QT += widgets

CONFIG += c++17
CONFIG += utf8_source

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CustomEvent.cpp \
    MyApplication.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    CustomEvent.h \
    MainWindow.h \
    MyApplication.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
