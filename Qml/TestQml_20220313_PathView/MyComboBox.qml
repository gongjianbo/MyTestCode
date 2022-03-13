import QtQuick 2.12
import QtQuick.Controls 2.12

//环形选择框
//龚建波 2022-03-13
//note：弹框为pop会被限制在window内
ComboBox {
    id: control

    implicitWidth: 30
    implicitHeight: 30
    opacity: 0.9999

    delegate: ItemDelegate {
        width: 30
        height: width
        padding: 0
        background: Rectangle {
            radius: width / 2
            color: "green"
            border.color: "black"
        }
        contentItem: Text {
            text: modelData
            padding: 0
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
    contentItem: Text {
        text: control.displayText
        padding: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    indicator: null
    background: Rectangle {
        radius: 15
        color: "green"
        border.color: "black"
    }
    popup: Popup {
        id: pop
        width: 200
        height: width
        anchors.centerIn: parent
        margins: 0
        padding: 0
        //pathview环形的角度范围和延申半径
        property int angle: 1
        property int spread: 1
        //pop弹出和隐藏时的过渡动画
        enter: Transition {
            ParallelAnimation {
                NumberAnimation { property: "angle"; from: 1; to: 360; duration: 500 }
                NumberAnimation { property: "spread"; from: 1; to: 100; duration: 500 }
            }
        }
        exit: Transition {
            ParallelAnimation {
                NumberAnimation { property: "angle"; from: 360; to: 1; duration: 500 }
                NumberAnimation { property: "spread"; from: 100; to: 1; duration: 500 }
            }
        }
        background: Item { }
        contentItem: PathView {
            model: control.popup.visible ? control.delegateModel : null
            //currentIndex: control.highlightedIndex
            //highlightRangeMode: PathView.NoHighlightRange
            interactive: false
            path: Path {
                //一个圆环路径
                PathAngleArc {
                    centerX: 100; centerY: 100
                    radiusX: pop.spread; radiusY: pop.spread
                    moveToStart: true
                    startAngle: 0
                    sweepAngle: pop.angle
                }
            }
        }
    }
}
