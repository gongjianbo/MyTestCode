import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("点击进度条开始动画")
    color: "white"

    CircleProgress{
        id: circle_progress
        anchors.centerIn: parent
        width: 160
        height: 160

        MouseArea{
            anchors.fill: parent
            onClicked: progress_ani.start()
        }

        NumberAnimation{
            id: progress_ani
            target: circle_progress
            property: "progress"
            from: 0
            to: 1
            duration: 3000
            running: false
            loops: 1//Animation.Infinite
        }
    }
}
