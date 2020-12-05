import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Component 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("GongJianBo 1992")

    Shortcut{
        sequence: "space"
        onActivated: {
            console.log("shortcut active space")
        }
    }

    Row{
        spacing: 10
        Button{
            focus: true
            text: "a"
            onClicked: console.log("click",text)
            onPressed: console.log("press",text)

            //测试自定义Keys
            //KeysFilter.enabled: false
            KeysFilter.filterKeys: [Qt.Key_Space]
            //KeysFilter.acceptShortcut: false

            //测试Qt的Keys
            //在5.12 5.13里accepted后组件只过滤了press，click还是会触发
            /*Keys.onSpacePressed: {
                console.log("key space")
                event.accepted=false;
            }
            Keys.onShortcutOverride: {
                console.log("key shortcut")
                event.accepted=false;
            }*/
        }

        Button{
            text: "b"
            onClicked: console.log("click",text)
            onPressed: console.log("press",text)
        }

        Button{
            text: "c"
            onClicked: console.log("click",text)
            onPressed: console.log("press",text)
        }
    }
}
