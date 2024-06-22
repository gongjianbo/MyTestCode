import QtQuick 2.15
import Test 1.0

// 消息框管理
// 龚建波 2024-06-22
Item {
    id: control

    // 弹出新的消息时，信号通知已有的消息框上移
    signal showNext()

    property var toastArray: []

    Component {
        id: toast_comp
        ToastItem {
            id: toast_item
            Connections {
                target: control
                function onShowNext() {
                    toast_item.next()
                }
            }
            onClosing: {
                toastArray.shift()
                toast_item.destroy()
            }
            Component.onDestruction: {
                console.log("onDestruction")
            }
            Component.onCompleted: {
                console.log("onCompleted")
            }
        }
    }

    ScreenTool {
        id: screen_tool
    }

    function showToast(msg) {
        let rect = screen_tool.focusRect()
        if (rect.width <= 0)
            return
        control.showNext()
        let toast = toast_comp.createObject()
        // 存起来避免被gc
        toastArray.push(toast)
        toast.pop(msg, rect)
    }
}
