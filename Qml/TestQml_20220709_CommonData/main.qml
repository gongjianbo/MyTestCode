import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 900
    height: 500
    visible: true
    title: qsTr("Shared Data")

    DataView {
        width: parent.width / 2
        height: parent.height
        color: "lightGray"
    }

    DataView {
        x: parent.width / 2
        width: parent.width / 2
        height: parent.height
        color: "darkGray"
    }
}
