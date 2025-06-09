QT -= gui

TEMPLATE = lib
DESTDIR = $$PWD/../lib

CONFIG += staticlib

CONFIG += c++11 utf8_source console

QMAKE_CFLAGS_WARN_ON = /W3
QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
QMAKE_CXXFLAGS += /sdl

DEFINES += NOMINMAX
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    MessageObject.h \
    ProcessDefine.h \
    ProcessManager.h \
    ProcessWorker.h

SOURCES += \
    MessageObject.cpp \
    ProcessManager.cpp \
    ProcessWorker.cpp

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
