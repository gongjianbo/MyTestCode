import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 300
    height: 500
    visible: true
    title: qsTr("Hello CMake")

    ListView{
        anchors.fill: parent
        anchors.margins: 6
        spacing: 2
        model: 20
        delegate: Rectangle{
            width: ListView.view.width
            height: 30
            color: Qt.hsla(Math.random(),1.0,0.5,1.0);
        }
    }

}
