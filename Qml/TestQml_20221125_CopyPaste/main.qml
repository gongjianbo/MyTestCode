import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Copy Paste")

    GridLayout {
        anchors.fill: parent
        anchors.margins: 12
        rowSpacing: 12
        columnSpacing: 12
        rows: 2
        columns: 2
        ImageArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        ImageArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        ImageArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        ImageArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
