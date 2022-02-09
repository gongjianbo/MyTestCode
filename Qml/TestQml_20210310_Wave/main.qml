import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Qml Wave")

    Row {
        spacing: 20

        //勾选递增后定时器增加数值
        CheckBox {
            id: auto_increment
            text: "递增"
            checked: true
        }
    }

    CanvasWave {
        id: wave
        anchors.centerIn: parent
        //waveColor: "darkCyan"

        //用来测试边界
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "black"
        }
    }

    Timer {
        id: timer
        running: auto_increment.checked
        repeat: true
        interval: 30
        onTriggered:{
            //测试用逻辑，递增进度值
            if(wave.curValue <= wave.maxValue){
                wave.curValue += 0.1;
            }

            if(wave.curValue > wave.maxValue){
                wave.curValue = wave.minValue;
            }
        }
    }
}
