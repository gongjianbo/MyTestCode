import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtAV 1.7

Window {
    id:item_root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    color: "gray"

    VideoOutput2 {
        id: item_video
        source: item_player
        anchors.fill: parent
        anchors.bottomMargin: 30

        Slider{
            id:item_slider
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            from:0
            value:0
            to:1
            onValueChanged: {
                if(item_slider.theHover)
                    item_player.seek(item_player.duration*value)
                //console.log(item_player.duration)
            }

            property  bool theHover: false
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                hoverEnabled: true
                onEntered: item_slider.theHover=true
                onExited: item_slider.theHover=false
            }
        }
    }

    MediaPlayer{
        id:item_player
        //替换成自己的文件或者网络视频流
        source: "C:/Users/zhaozhao/Videos/Avicii.mp4"
        onPaused: {
            console.log("----------paused")
        }
        onPlaying: {
            console.log("----------playing")
        }
        onStopped: {
            console.log("----------stopped")
        }
        onPositionChanged: {
            if(!item_slider.theHover)
                item_slider.value=position/duration
            console.log("----------buffer position",position)
        }
    }


    Row{
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 2
        height: 30
        spacing: 2
        Button{
            anchors.verticalCenter: parent.verticalCenter
            width: 60
            height: 26
            text: "play"
            onClicked: {
                console.log("-------------play clicked")
                item_player.play()
            }
        }

        Button{
            anchors.verticalCenter: parent.verticalCenter
            width: 60
            height: 26
            text: "pause"
            onClicked: {
                console.log("-------------pause clicked")
                item_player.pause()
            }
        }
        Button{
            anchors.verticalCenter: parent.verticalCenter
            width: 60
            height: 26
            text: "stop"
            onClicked: {
                console.log("-------------stop clicked")
                item_player.stop()
                item_slider.value=0
            }
        }
    }
}
