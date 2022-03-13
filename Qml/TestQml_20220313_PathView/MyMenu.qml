import QtQuick 2.12
import QtQuick.Controls 2.12

//环形菜单
//龚建波 2022-03-13
//note：弹框为pop会被限制在window内
Menu {
    id: control

    implicitWidth: 250
    implicitHeight: 250
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
    delegate: MenuItem {
        id: item
        width: 30
        height: width
        padding: 0
        spacing: 0
        indicator: null
        arrow: null
        background: Rectangle {
            radius: width / 2
            color: "red"
            border.color: "black"
        }
        contentItem: Text {
            text: item.text
            padding: 0
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
    contentItem: PathView {
        implicitWidth: 250
        implicitHeight: 250
        model: control.contentModel
        //把PathView放Menu，会有一个高亮Item被放到contentModel，减去
        pathItemCount: control.count > 0 ? control.count - 1 : 0
        //currentIndex: control.currentIndex
        //highlightRangeMode: PathView.NoHighlightRange
        interactive: false
        path: Path {
            //一个圆环路径
            PathAngleArc {
                centerX: 125; centerY: 125
                radiusX: control.spread; radiusY: control.spread
                moveToStart: true
                startAngle: 0
                sweepAngle: control.angle
            }
        }
    }
    background: Item { }
}
