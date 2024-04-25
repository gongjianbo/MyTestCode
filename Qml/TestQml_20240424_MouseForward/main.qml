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
        // 最下层MouseArea
        MouseArea {
            id: bottom
            anchors.fill: parent
            hoverEnabled: true
        }

        Rectangle {
            x: 100
            y: 100
            width: 200
            height: 200
            color: "transparent"
            border.color: top.containsMouse ? "red" : "black"
            Text {
                anchors.centerIn: parent
                text: String("(%1,%2)").arg(top.mouseX).arg(top.mouseY)
            }
            // 自定义MouseArea，收到事件后转发给下层MouseArea
            MyMouseArea {
                id: top
                anchors.fill: parent
                hoverEnabled: true
                forwardTo: bottom
            }

            Rectangle {
                x: 100
                y: 100
                width: 200
                height: 200
                color: "transparent"
                border.color: topmost.containsMouse ? "red" : "black"
                Text {
                    anchors.centerIn: parent
                    text: String("(%1,%2)").arg(topmost.mouseX).arg(topmost.mouseY)
                }
                // 用普通MouseArea，然后事件过滤后转发
                MouseArea {
                    id: topmost
                    anchors.fill: parent
                    hoverEnabled: true
                }
                MyMouseFilter {
                    attach: topmost
                    forwardTo: top
                }
            }
        }
    }
}
