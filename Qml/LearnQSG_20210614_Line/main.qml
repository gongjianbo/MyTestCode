import QtQuick 2.12
import QtQuick.Window 2.12
import Gt.Component 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Learn QSG")
    color: "darkGreen"

    QSGLine{
        width: parent.width/2
        height: parent.height
        //通过多重采样来实现线条抗锯齿
        //layer.enabled: true
        //layer.samples: 32
    }

    MyLine{
        x: parent.width/2
        width: parent.width/2
        height: parent.height
    }

    SmoothLine{
        visible: false
        anchors.fill: parent
        anchors.margins: 10
    }
}
