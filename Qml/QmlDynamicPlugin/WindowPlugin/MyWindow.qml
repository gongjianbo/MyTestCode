import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: control
    width: 600
    height: 400
    //modality: Qt.ApplicationModal
    objectName: "WindowPlugin"
    title: "Plugin Windwow"

    signal sendMessage(string message)

    Column {
        spacing: 12
        anchors.centerIn: parent
        Button {
            id: btn
            text: "Send"
            onClicked: {
                control.sendMessage(edit.text)
            }
        }
        TextField {
            id: edit
            text: "I'm Plugin"
            selectByMouse: true
        }
    }

    function recvMessage(message)
    {
        edit.text = message
    }
}
