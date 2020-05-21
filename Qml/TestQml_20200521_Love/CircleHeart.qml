import QtQuick 2.12
import QtQuick.Particles 2.12

Item {
    id: control

    ParticleSystem {
        id: sys
        width: 300
        height: 300
        anchors.centerIn: parent

        ImageParticle {
            anchors.fill: parent
            system: sys
            source: "qrc:/heart_1.png"
            color: Qt.rgba(0,0.5,0,0.8)
            //colorVariation: 0.5 //彩色效果
        }

        Emitter {
            id: emitter
            //anchors.fill: parent
            system: sys

            emitRate: 10  //发射速率
            lifeSpan: 5000 //生存周期ms
            size: 25 //尺寸
            sizeVariation: 5
            // 移动效果
            velocityFromMovement: 8
            velocity: PointDirection {xVariation: 2; yVariation: 2;}
            acceleration: PointDirection {xVariation: 10; yVariation: 10;}

            x:circle.cx
            y:circle.cy
        }

        Rectangle{
            //测试范围用的矩形
            z: -1
            anchors.fill: parent
            border.color: "green"
            color: "transparent"
        }
    }

    Item {
        id: circle
        property real radius: 100
        property real cx: radius * Math.sin(percent*2*Math.PI)+sys.width/2
        property real cy: radius * Math.cos(percent*2*Math.PI)+sys.height/2
        property real percent: 0

        SequentialAnimation on percent {
            loops: Animation.Infinite
            running: true
            NumberAnimation { from: 0.0; to: 1 ; duration: 5000;  }
        }
    }
}
