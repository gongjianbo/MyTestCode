import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Qml EventLoop")

    Rectangle{
        width: 300
        height: 300
        border.color: "red"
        anchors.centerIn: parent
        Text{
            text: "hover时显示自定义样式"
            anchors.centerIn: parent
            ToolTip.text: "自定义样式"
            ToolTip.visible: mouse.containsMouse
            ToolTip{
                id: tip
                text: "默认样式"
                visible: !mouse.containsMouse
            }
        }
        MouseArea{
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
        }
    }
}
