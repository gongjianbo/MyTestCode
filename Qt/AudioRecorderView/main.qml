import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Test 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("龚建波 1992")

    AudioRecorderView{
        id: recorder
        anchors.fill: parent
        anchors.topMargin: 50
    }

    Row{
        spacing: 10
        Button{
            text: "record"
            onClicked: recorder.record(16000)
        }
        Button{
            text: "stop"
            onClicked: recorder.stop()
        }
        Button{
            text: "play"
            onClicked: {
                recorder.output.resetToDefaultDevice()
                recorder.play()
            }
        }
        Button{
            text: "save"
            onClicked: recorder.saveToFile("./save.wav")
        }
        Button{
            text: "load"
            onClicked: recorder.loadFromFile("./save.wav")
        }
        Label{
            anchors.verticalCenter: parent.verticalCenter
            text: "state:"+recorder.recordState
        }
    }
}
