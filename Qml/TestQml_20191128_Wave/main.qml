import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 720
    height: 520
    title: qsTr("Hello World")

    color: "black"

    EffectsWave{
        width: 200
        height: 200
        x:100
        y:100
    }

    ShapesWave{
        width: 200
        height: 200
        x:300
        y:100
    }

    ClickWave{
        id: click_wave
        width: 100
        height: 100
        x:100
        y: 300
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            click_wave.click(mouse.x,mouse.y)
        }
    }
}
