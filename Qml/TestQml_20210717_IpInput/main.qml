import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Ip Address --GongJianBo 1992")

    IpInput{
        anchors.centerIn: parent
        width: 300
        height: 40
    }

    /*Column{
        x: 10
        y: 10
        spacing: 10

        Rectangle{
            width: 300
            height: 40
            border.color: "black"
            TextInput{
                id: time_input
                anchors.fill: parent
                selectByMouse: true
                inputMask: "99:99:99.999"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: RegExpValidator {
                    regExp:/^([0-1\s]?[0-9\s]|2[0-3\s]):([0-5\s][0-9\s]):([0-5\s][0-9\s]).([0-9][0-9][0-9])$/
                }
                verticalAlignment: TextInput.AlignVCenter
                leftPadding: 10
                Keys.enabled: true
                Keys.onReturnPressed: {
                    console.log(time_input.text)
                }
            }
        }

        Rectangle{
            width: 300
            height: 40
            border.color: "black"
            TextInput{
                id: ip_input
                anchors.fill: parent
                selectByMouse: true
                inputMask: "999-999-999-999"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: RegExpValidator {
                    regExp:/^([0-2\s]?[0-9\s]?[0-9\s])-([0-2\s]?[0-9\s]?[0-9\s])-([0-2\s]?[0-9\s]?[0-9\s])-([0-2\s]?[0-9\s]?[0-9\s])$/
                }
                verticalAlignment: TextInput.AlignVCenter
                leftPadding: 10
                Keys.enabled: true
                Keys.onReturnPressed: {
                    console.log(ip_input.text)
                }
            }
        }
    }*/
}
