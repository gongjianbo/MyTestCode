import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

//环形进度条
//龚建波 2021-1-7
Item {
    id: control
    implicitWidth: 160
    implicitHeight: 160

    //样式只有两个主体颜色
    //浅色
    property color lightColor: "#73C3F3"
    //深色
    property color darkColor: "#22A7F0"
    //光影
    property color glowColor: Qt.rgba(0.7,0.85,0.95,0.4)
    //文本颜色
    property color textColor: "#FFFFFF"
    //圆圈线宽
    property int borderWidth: 16
    //光影宽度
    property int marginWidth: 10

    //进度值 [0,1]
    property real progress: 0
    //已完成
    property bool isFinish: progress>=1

    Item{
        anchors.fill: parent
        anchors.margins: control.marginWidth

        //外层圆圈
        Rectangle{
            id: circle_outer
            anchors.fill: parent
            radius: width/2
            border.width: control.borderWidth
            border.color: control.lightColor
            color: "transparent"
            layer.enabled: true
            layer.effect: Glow{
                color: control.glowColor
                radius: control.marginWidth
                samples: 8
                spread: 0.4
            }
        }

        //内层圆圈
        Item{
            id: circle_inner
            anchors.fill: parent
            anchors.margins: control.borderWidth+8
            Item{
                width: parent.width
                height: parent.height*control.progress
                anchors.bottom: parent.bottom
                clip: true
                Rectangle{
                    width: circle_inner.width
                    height: circle_inner.height
                    radius: width/2
                    anchors.bottom: parent.bottom
                    color: control.lightColor
                }
            }
        }

        //外层旋转的半圆圈
        Item{
            id: circle_outerrun
            anchors.fill: parent
            Item{
                width: parent.width/2
                height: parent.height
                clip: true
                Rectangle{
                    width: circle_outer.width
                    height: circle_outer.height
                    radius: width/2
                    border.width: control.borderWidth
                    border.color: control.darkColor
                    color: "transparent"
                }
            }

            RotationAnimation{
                target: circle_outerrun
                from: 0
                to: 360
                duration: 1000
                running: circle_outerrun.visible
                loops: Animation.Infinite
            }
        }

        //已完成后的圆圈
        Rectangle{
            visible: control.isFinish
            anchors.fill: parent
            radius: width/2
            color: control.darkColor

            Text {
                anchors.centerIn: parent
                color: control.textColor
                text: "Finish"
                font.family: "微软雅黑"
                font.pixelSize: 20
            }
        }
    }
}
