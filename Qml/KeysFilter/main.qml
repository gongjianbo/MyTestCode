import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Component 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("GongJianBo 1992")

    Row{
        spacing: 10
        Button{
           focus: true
           text: "a"
           onClicked: console.log("click",text)
           onPressed: console.log("press",text)

           KeysFilter.filterKeys: [Qt.Key_Space]
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
