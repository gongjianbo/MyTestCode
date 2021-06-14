import QtQuick 2.12
import QtQuick.Window 2.12
import Gt.Component 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Learn QSG")

    MyLine{
        anchors.fill: parent
        anchors.margins: 10
        //通过多重采样来实现线条抗锯齿
        //layer.enabled: true
        //layer.samples: 32
    }
}
