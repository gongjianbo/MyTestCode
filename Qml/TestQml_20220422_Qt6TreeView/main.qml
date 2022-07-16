import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Qt6 TreeView")

    MyTreeView {
        anchors.fill: parent
        anchors.margins: 10
    }
}
