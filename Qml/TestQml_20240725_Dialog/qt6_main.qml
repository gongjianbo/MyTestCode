import QtQuick
import QtQuick.Controls as QC2

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("QML Dialog")
    color: "orange"
    onClosing: (event)=>{
                   console.log("Window root closing")
                   event.accepted = true
               }

    Row {
        spacing: 10
        QC2.Button {
            text: "qc2"
            onClicked: {
                qc2.open()
            }
        }
        QC2.Button {
            text: "win"
            onClicked: {
                win.show()
            }
        }
    }
    // 叠加 Dialog 时，内部 Dialog 需要 parent 可见才能显示
    // 安卓返回时 Dialog 和 Window 的 close 都会触发，不好解决
    QC2.Dialog {
        id: qc2
        width: 200
        height: 200
        modal: true
        padding: 0
        // closePolicy 会影响 ESC 响应以及安卓返回处理
        // 默认 Popup.CloseOnEscape | Popup.CloseOnPressOutside
        // closePolicy: QC2.Dialog.NoAutoClose
        Rectangle {
            anchors.fill: parent
            color: "red"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    qc2_sub.open()
                }
            }
            // 内部嵌套弹框
            // 逐层处理 ESC 和安卓返回
            QC2.Dialog {
                id: qc2_sub
                // closePolicy: QC2.Dialog.NoAutoClose
                width: 100
                height: 100
                Rectangle {
                    anchors.fill: parent
                    color: "cyan"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            qc2_sub.close()
                            qc2.close()
                        }
                    }
                }
                onClosed: {
                    console.log("Dialog sub close")
                }
            }
        }
        onClosed: {
            console.log("Dialog close")
        }
    }
    // 叠加 Window，安卓返回会依次处理 close
    // 没有模态阴影和点外部自动关闭的效果，但是可以自定义
    Window {
        id: win
        width: 200
        height: 200
        // 默认 WindowModal
        // 模态设置不影响安卓返回触发
        modality: Qt.NonModal
        // 安卓上 Window 默认全屏，所以用透明背景的方式来显示，但是不能点击外层组件
        color: "transparent"
        Rectangle {
            anchors.centerIn: parent
            width: 200
            height: 200
            color: "blue"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    win_sub.show()
                }
            }
            Window {
                id: win_sub
                width: 200
                height: 200
                modality: Qt.NonModal
                color: "transparent"
                Rectangle {
                    anchors.centerIn: parent
                    width: 100
                    height: 100
                    color: "pink"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            win_sub.close()
                            win.close()
                        }
                    }
                }
                onClosing: (event)=>{
                               console.log("Window sub closing")
                           }
            }
        }
        onClosing: (event)=>{
                       console.log("Window closing")
                   }
    }
}
