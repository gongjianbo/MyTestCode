import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("PathView")

    Row {
        anchors.centerIn: parent
        spacing: 20

        MyComboBox {
            model: 10
        }

        Button {
            width: 60
            height: 30
            text: "menu"
            background: Rectangle {
                radius: 15
                color: "red"
                border.color: "black"
            }
            onClicked: {
                menu.popup()
            }

            MyMenu {
                id: menu
                anchors.centerIn: parent
                Action { text: "1" }
                Action { text: "2" }
                Action { text: "3" }
                Action { text: "4" }
                Action { text: "5" }
                Action { text: "6" }
                Action { text: "7" }
                Action { text: "8" }
                Action { text: "9" }
                Action { text: "10" }
            }
        }
    }
}
