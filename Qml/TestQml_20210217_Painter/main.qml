import QtQuick 2.12
import QtQuick.Window 2.12
import Test 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("GongJianBo 1992")

    Rectangle{
        anchors.fill: parent
        anchors.margins: 20
        border.color: "red"

        QmlPaintDevice{
            id: device
            anchors.fill: parent
            property int value: 0

            //目前只找到两种方式，paint里回调qml函数进行绘制，或者before时发送信号给qml绘制
            //主要问题在于qsg渲染在独立线程
            //我中意的就是第二种方式，但是需要把原本的qquickpainteditem重写一些接口
            function doPaint(painter){
            //onDoPaint: {
                painter.drawLine(10,10+200-value,width-10,10+value);
            }
            onValueChanged: update();
        }

        Timer{
            running: true
            repeat: true
            interval: 20
            onTriggered: {
                //console.log("update",device.value)
                device.value+=1;
                if(device.value>200)
                    device.value=0;
            }
        }

        /*NumberAnimation{
            target: device
            property: "value"
            from:0
            to:200
            duration: 2000
            running: true
            loops: Animation.Infinite
        }*/
    }
}
