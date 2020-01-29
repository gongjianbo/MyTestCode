QT += quick

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        FBOItem.cpp \
        FBORenderer.cpp \
        FBORenderer2.cpp \
        main.cpp

HEADERS += \
    FBOItem.h \
    FBORenderer.h \
    FBORenderer2.h

RESOURCES += qml.qrc
