QT += core
QT += gui
QT += widgets
QT += opengl
greaterThan(QT_MAJOR_VERSION, 5){
QT += openglwidgets
}

CONFIG += c++11
CONFIG += utf8_source

DESTDIR = $$PWD/bin

win32{
    LIBS += -lOpenGL32
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    FixedGLWidget.cpp \
    ProgramGLWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    FixedGLWidget.h \
    ProgramGLWidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    res.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
