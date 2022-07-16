import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import Gt.Component 1.0

//渲染纹理的Item坐标不能带小数，不然会模糊
Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("QML Texture Render Bug")
    color: "black"

    //两个参照物
    Column {
        x: 10
        y: 10
        spacing: 10
        Text {
            text: qsTr("测试一下\nTest Ok?")
            font.pixelSize: 14
            color: "white"
        }
        PaintedItem {
            width: 100
            height: 100
        }
    }

    //【1】作为 shader 的纹理
    Text {
        id: txt2
        text: qsTr("测试一下\nTest Ok?")
        font.pixelSize: 14
        color: "white"
        visible: false
    }
    ShaderEffectSource {
        id: txt2source
        sourceItem: txt2
    }

    ShaderEffect {
        x: 200.5
        y: 10.5
        width: txt2.width
        height: txt2.height
        property var source: txt2source
    }

    //【2】layer 生成纹理
    Text {
        id: txt3
        x: 200.5
        y: 200.5
        text: qsTr("测试一下\nTest Ok?")
        font.pixelSize: 14
        color: "white"
        layer.enabled: true
    }

    //【3】 painted 本身就绘制到一个纹理上
    PaintedItem {
        x: 10.5
        y: 200.5
        width: 100
        height: 100
    }

    //组合测试，一个弹框
    MouseArea {
        anchors.fill: parent
        onClicked: {
            pop.x  = 200.5;
            pop.y  = 200.5;
            pop.open();
        }
    }

    Popup {
        id: pop
        padding: 0
        Rectangle {
            id: rect
            width: 200
            height: 150
            color: "black"
            border.color: "red"
            Text {
                anchors.centerIn: parent
                text: qsTr("测试一下\nTest Ok?")
                font.pixelSize: 14
                color: "white"
                //使用layer或者shader-shadow都会生成纹理
                //layer.enabled: true
            }
        }
        DropShadow {
            anchors.fill: rect
            horizontalOffset: 1
            verticalOffset: 1
            radius: 6
            samples: 8
            color: "green"
            source: rect
        }
    }
}
