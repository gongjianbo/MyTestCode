import QtQuick 2.12
import QtQuick.Controls 2.12

//分页组件上的页码按钮
//龚建波 2020-12-20
Button {
    id: control

    //该按钮代表的页码
    property int pageNumber: -2
    //当前选中的页码
    property int pageCurrent: -3
    property color textColor: "#666666"
    property color highlightedColor: "#1989FA"

    implicitWidth: implicitContentWidth+leftPadding+rightPadding
    implicitHeight: implicitContentHeight+topPadding+bottomPadding
    padding: 0
    text: pageNumber

    contentItem: Text {
        text: control.text
        font: control.font
        color: pageNumber===pageCurrent?highlightedColor:textColor
        opacity: enabled ? 1.0 : 0.3
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    //暂时没有背景，可以加一个小圆圈背景
    background: Item {
    }
}
