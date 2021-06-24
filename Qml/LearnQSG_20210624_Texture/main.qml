import QtQuick 2.12
import QtQuick.Window 2.12
import Gt.Component 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Learn QSG")

    Rectangle{
        width: 100
        height: 100
        border.color: "green"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                //txt.text = "QSG"
                blender.source = txt2;
            }
        }
    }

    Text{
        id: txt
        font.pixelSize: 40
        text: "Hello"
        //visible: false
        layer.enabled: true
    }

    Text{
        id: txt2
        font.pixelSize: 40
        text: "QSG"
        //visible: false
        layer.enabled: true
    }

    MyBlender{
        id: blender
        x: 100
        y: 100
        width: source.width
        height: source.height
        source: txt
    }
}
