import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

// 消息提示框
// 龚建波 2024-06-22
Window {
    id: control

    // width: context.implicitWidth + 40
    // height: context.implicitHeight + 30
    y: yShow - yOffset
    flags: Qt.ToolTip
    color: "transparent"

    // 移动起始位置
    property real yFrom: 0
    // 移动暂留位置
    property real yStay: 0
    // 移动结束位置
    property real yTo: 0
    // 移动当前位置
    property real yShow: 0
    // 位置偏移
    property real yOffset: 0

    // 背景
    Rectangle {
        id: bg
        anchors.fill: parent
        radius: 4
        color: "#99000000"
    }

    // 文字内容
    Text {
        id: context
        anchors.centerIn: parent
        color: "white"
    }

    Behavior on yOffset {
        NumberAnimation {
            duration: 200
        }
    }

    SequentialAnimation {
        id: sequential
        ParallelAnimation {
            NumberAnimation {
                target: control
                properties: "opacity"
                from: 0.1
                to: 1
                duration: 200
            }
            NumberAnimation {
                target: control
                properties: "yShow"
                from: control.yFrom
                to: control.yStay
                duration: 200
            }
        }
        NumberAnimation {
            target: control
            properties: "opacity"
            to: 1
            duration: 2500
        }
        ParallelAnimation {
            NumberAnimation {
                target: control
                properties: "opacity"
                to: 0.1
                duration: 200
            }
            NumberAnimation {
                target: control
                properties: "yShow"
                to: control.yTo
                duration: 200
            }
        }
        onFinished: {
            control.close()
        }
    }

    // 弹出消息框
    function pop(tipText, screenRect) {
        context.text = tipText

        control.width = context.implicitWidth + 40
        control.height = context.implicitHeight + 20
        control.x = screenRect.x + (screenRect.width - control.width) / 2
        control.yStay = screenRect.y + (screenRect.height - control.height) / 4
        control.yFrom = control.yStay + 80
        control.yTo = control.yStay - 80
        control.opacity = 0.1
        control.yShow = control.yFrom
        control.show()
        sequential.start()
    }

    // 需要显示下一个消息，位置上移
    property int nextCount: 0
    function next() {
        // 限制同时显示的个数，多余的关闭
        if (nextCount >= 2) {
            control.close()
            return
        }
        nextCount++
        // 消息框之间保留间隔
        control.yOffset = nextCount * (control.height + 10)
    }
}
