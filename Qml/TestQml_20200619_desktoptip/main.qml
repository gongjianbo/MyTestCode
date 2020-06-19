import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("龚建波 1992")
    color: "green"

    MouseArea{
        anchors.fill: parent
        onClicked: pop.showTip()
    }

    DesktopTip{
        id: pop
        title: qsTr("DesktopTip")
        content: Rectangle{
            width: 300
            height: 200
            color: Qt.rgba(1,0,0,0.7)
            Text {
                anchors.centerIn: parent
                text: qsTr("DesktopTip")
            }
        }
    }
}
