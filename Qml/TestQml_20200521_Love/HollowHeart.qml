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
            anchors.fill: parent
            system: sys
            //maximumEmitted: 500
            emitRate: 100  //发射速率
            lifeSpan: 2500 //生存周期ms
            size: 25 //尺寸
            endSize: 40  //最终尺寸
            sizeVariation: 5
            // 移动效果
            acceleration: AngleDirection{
                magnitude: 10
                angleVariation: 360
            }
            // mask只能用source
            shape: MaskShape {
                source: "qrc:/heart_2.png" //空心
            }
        }

    }
}
