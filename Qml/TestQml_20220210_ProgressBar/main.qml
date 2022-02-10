import QtQuick 2.12
import QtQml 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Progress")

    Button {
        id: btn
        text: "run"
        onClicked: {
            na.start()
        }
    }

    property int value: 0

    NumberAnimation {
        id: na
        target: root
        property: "value"
        from: 0
        to: 100
        duration: 3000
        loops: 1
        running: true
    }

    Row {
        anchors.centerIn: parent
        spacing: 50
        ProgressCircle {
            value: root.value
        }
        ProgressWave {
            value: root.value
        }
    }
}
