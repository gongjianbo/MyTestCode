import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Column{
        x:10
        y:10
        spacing: 10
        Button{
            id: btn_1
            text: "Dialog 1"
            onClicked: {
                dialog1.text+=dialog1.text
                dialog1.open()
            }
        }

        Button{
            id: btn_2
            text: "Dialog 1"
            onClicked: {
                dialog2.open()
            }
        }
    }

    MessageWindow{
        id: dialog1
        title: "这里是标题"
        text: "每次open文本叠加一倍测试高度适应。"
        buttonFlags: MessageWindow.CancelButton
                     |MessageWindow.OkButton
    }

    MessageWindow{
        id: dialog2
        text: "测试 buttonFlags 枚举。"
        buttonFlags: MessageWindow.CancelButton
                     |MessageWindow.OkButton
                     |MessageWindow.CloseButton
    }

    Component.onCompleted: {
        console.log("enum ok=",MessageWindow.OkButton)
    }
}
