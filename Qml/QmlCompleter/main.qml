import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Qml Completer")

    CompleteTextField {
        anchors.centerIn: parent
        width: 500
        height: 30
        keywords: ["123456", "234567", "345678", "456789"]
    }
}
