import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Ripple")
    color: "#111111"

    Row{
        anchors.centerIn: parent
        spacing: 20
        ShaderRipple{
            width: 200
            height: 200
            Rectangle{
                anchors.fill: parent
                color: "transparent"
                border.color: "red"
            }
        }
        GradientRipple{
            width: 200
            height: 200
            Rectangle{
                anchors.fill: parent
                color: "transparent"
                border.color: "red"
            }
        }
    }
}
