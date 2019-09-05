import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 800
    height: 500
    color: "#b1c5d2"
    title: qsTr("Hello World")

    AbnormalSignal{
        anchors.fill: parent
        anchors.margins: 10
    }
}
