import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("QML FPS")

    QmlFps {
        anchors.centerIn: parent
    }

    Rectangle {
        anchors.centerIn: parent
        width: 120
        height: 120
        color: "transparent"
        border.color: "red"
        RotationAnimation on rotation {
            from: 0
            to: 360
            duration: 3000
            loops: Animation.Infinite
        }
    }
}
