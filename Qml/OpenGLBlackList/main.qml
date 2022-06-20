import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("QML Window")
    color: "darkGray"

    Button {
        anchors.centerIn: parent
        text: "Pop"
        onClicked: {
            console.log('pop window')
            win2.show()
        }
    }

    Window {
        id: win2
        width: 400
        height: 320
        color: "gray"
    }
}
