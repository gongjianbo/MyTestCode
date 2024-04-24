import QtQuick 2.15
import QtQuick.Window 2.15
import Test 1.0

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("GongJianBo")

    Rectangle {
        width: 200
        height: 200
        color: "transparent"
        border.color: bottom.containsMouse ? "red" : "black"
        Text {
            anchors.centerIn: parent
            text: String("(%1,%2)").arg(bottom.mouseX).arg(bottom.mouseY)
        }
        MyMouseArea {
            id: bottom
            anchors.fill: parent
            hoverEnabled: true
        }

        Rectangle {
            x: 150
            y: 150
            width: 200
            height: 200
            color: "transparent"
            border.color: top.containsMouse ? "red" : "black"
            Text {
                anchors.centerIn: parent
                text: String("(%1,%2)").arg(top.mouseX).arg(top.mouseY)
            }
            MyMouseArea {
                id: top
                anchors.fill: parent
                hoverEnabled: true
                forwardTo: bottom
            }
        }
    }
}
