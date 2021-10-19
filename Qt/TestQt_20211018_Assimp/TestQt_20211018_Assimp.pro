QT += core gui widgets
CONFIG += c++11 utf8_source
DESTDIR = $$PWD/bin

SOURCES += \
    GLView.cpp \
    main.cpp

# MSVC2019 64bit md-release
LIBS += -L$$PWD/assimp/lib/ -lassimp-vc142-md

INCLUDEPATH += $$PWD/assimp
DEPENDPATH += $$PWD/assimp

HEADERS += \
    GLView.h
