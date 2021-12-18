QT += quick
CONFIG += c++11 utf8_source

DESTDIR = $$PWD/bin

SOURCES += \
        ListModel.cpp \
        Translator.cpp \
        main.cpp

HEADERS += \
    ListModel.h \
    Translator.h

TRANSLATIONS += trans_zh_CN.ts \
    trans_en_US.ts

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

