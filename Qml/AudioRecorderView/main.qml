import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Test 1.0

//测试组件的使用
//先录制再播放或者保存，保存后下次可以加载
Window {
    visible: true
    width: 850
    height: 600
    title: qsTr("QML录音 (BY:龚建波1992  QQ群:647637553)")

    AudioRecorderView{
        id: recorder
        anchors.fill: parent
        anchors.topMargin: 100
    }

    Row{
        spacing: 10

        //太挫了，插了耳机电脑就默认只有耳机的输入输出，没法测试多个io
        ComboBox{
            id: input_comb
            width: 250
            model: recorder.input.filterInputDevicesName
            popup.onAboutToShow: {
                recorder.input.checkInputDevices();
            }
        }
        ComboBox{
            id: output_comb
            width: 250
            model: recorder.output.filterOutputDevicesName
            popup.onAboutToShow: {
                recorder.output.checkOutputDevices();
            }
        }
        Label{
            anchors.verticalCenter: parent.verticalCenter
            text: "[duration]:"+recorder.durationString
        }
        Label{
            anchors.verticalCenter: parent.verticalCenter
            text: "[state]:"+recorder.recordState
        }
    }

    Row{
        y:50
        spacing: 10
        Button{
            text: "record"
            onClicked: recorder.record(16000,input_comb.currentText)
        }
        Button{
            text: "stop"
            onClicked: recorder.stop()
        }
        Button{
            text: "play"
            onClicked: {
                recorder.output.resetToDefaultDevice()
                recorder.play(output_comb.currentText)
            }
        }
        Button{
            text: "suspend"
            onClicked: recorder.suspendPlay()
        }
        Button{
            text: "resume"
            onClicked: recorder.resumePlay()
        }
        Button{
            text: "save"
            onClicked: recorder.saveToFile("./save.wav")
        }
        Button{
            text: "load"
            onClicked: recorder.loadFromFile("./save.wav")
        }      
    }

}
