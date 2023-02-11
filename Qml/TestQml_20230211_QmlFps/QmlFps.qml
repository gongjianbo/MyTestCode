import QtQuick 2.15
import QtQuick.Window 2.15

Rectangle {
    id: control
    width: 120
    height: 120
    color: "darkCyan"

    property int frameCounter: 0
    property int fps: 0

    Text {
        anchors.centerIn: parent
        font.pixelSize: 20
        color: "white"
        text: fps
    }

    Connections {
        target: control.Window.window
        function onAfterRendering()
        {
            frameCounter++
        }
    }

    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            fps = frameCounter
            frameCounter = 0
        }
    }
}
