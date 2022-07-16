import QtQuick 2.12
import QtQuick.Window 2.12
import OpenGLUnderQML 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("OpenGL Under QML")
    color: "red"

    Rectangle {
        width: 200
        height: 200
    }

    Squircle {
        x: 100; y: 100
        width: 200; height: 200
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
    }

    Rectangle {
        width: 200
        height: 200
        anchors.centerIn: parent
    }
}
