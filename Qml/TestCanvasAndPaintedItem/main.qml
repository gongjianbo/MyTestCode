import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Test 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Test Canvas and QQuickPaintedItem")

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "black"
            MyCanvas {
                id: my_cnavas
                anchors.fill: parent
                anchors.margins: 6
            }
            Text { x: 20; y: 20; text: "Canvas"; }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "black"
            MyPaintedItem {
                id: my_painted
                anchors.fill: parent
                anchors.margins: 6
            }
            Text { x: 20; y: 20; text: "QQuickPaintedItem"; }
        }
    }

    Timer {
        id: timer
        repeat: true
        running: true
        interval: 100
        property int rotation: 0
        onTriggered: {
            rotation++
            if (rotation > 360)
                rotation = 0
            my_cnavas.rotate(rotation)
            my_painted.rotate(rotation)
        }
    }
}
