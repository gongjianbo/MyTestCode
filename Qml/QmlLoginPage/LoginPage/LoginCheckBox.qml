import QtQuick 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Shapes 1.12

//自定义勾选框
T.CheckBox {
    id:control

    property color textColor: "black"
    property string note
    property color noteColor: "gray"
    property color indicatorColor: "black"

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)
    leftPadding: 5
    rightPadding: 5
    spacing: 5
    font{
        family: "SimSun"
        pixelSize: 13
    }

    indicator: Rectangle {
        implicitWidth: 14 //control.height-2*control.leftPadding
        implicitHeight: width
        x: control.leftPadding
        y: (parent.height-height) / 2
        color: "transparent"
        border.width: 1
        border.color: indicatorColor
        antialiasing: false
        radius: 3

        Shape {
            id: checked_indicator
            anchors.centerIn: parent
            width: parent.width-6
            height: parent.height-6
            visible: control.checkState === Qt.Checked
            //smooth: true
            //antialiasing: true
            ShapePath {
                strokeWidth: 2
                strokeColor: indicatorColor
                //fillRule: ShapePath.WindingFill
                fillColor: "transparent"
                startX: 0; startY: checked_indicator.height/2
                PathLine { x:checked_indicator.width/2; y:checked_indicator.height }
                PathLine { x:checked_indicator.width; y:0 }
            }
        }
        Rectangle {
            anchors.centerIn: parent
            width: parent.width/2
            height: parent.height/2
            color: indicatorColor
            antialiasing: false
            visible: control.checkState === Qt.PartiallyChecked
        }
    }

    contentItem: Row{
        leftPadding: control.indicator.width + control.spacing +control.leftPadding
        spacing: 6
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            font: control.font
            color: textColor
            renderType: Text.QtRendering
            //horizontalAlignment: Text.AlignHCenter
            //verticalAlignment: Text.AlignVCenter
            //renderType: Text.NativeRendering
            //elide: Text.ElideRight
            //leftPadding: control.indicator.width + control.spacing +control.leftPadding
            //rightPadding: control.rightPadding
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "("+control.note+")"
            font: control.font
            color: control.noteColor
            renderType: Text.QtRendering
        }
    }

    background: Item{ }
}
