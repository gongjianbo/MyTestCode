QT += quick

CONFIG += c++17 qmltypes utf8_source
QML_IMPORT_NAME = MyModel
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
    MyTreeModel.cpp \
    main.cpp

HEADERS += \
    MyTreeModel.h

resources.files = main.qml QmlTreeView.qml MyTreeView.qml
resources.prefix = /$${TARGET}
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
