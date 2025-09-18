import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Qt6 TreeView")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        Row {
            spacing: 10
            MyButton {
                text: "Clear"
                onClicked: tree_model.clear()
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "#B8B8B8"
            MyTreeView {
                anchors.fill: parent
                anchors.margins: 10
            }
        }
    }

    component MyButton : Button {
        implicitWidth: 90
        implicitHeight: 30
    }
}
