import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 as QC2
import QtQuick.Dialogs 1.3 as QD1

Window {
    id: win_root
    width: 640
    height: 480
    visible: true
    title: qsTr("QML Dialog")
    color: "orange"

    Row {
        spacing: 10
        QC2.Button {
            text: "qc2"
            onClicked: {
                qc2_pop.open()
            }
        }
        QC2.Button {
            text: "qd1"
            onClicked: {
                qd1_pop.open()
            }
        }
        QC2.Button {
            text: "win"
            onClicked: {
                win_pop.show()
            }
        }
    }
    // 叠加 Dialog 时，内部 Dialog 需要 parent 可见才能显示
    // 安卓返回时 Dialog 如果是 CloseOnPressOutside，Window 的 close 也会触发
    QC2.Dialog {
        id: qc2_pop
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
            // 如果内部是非模态 NoAutoClose 会把外部关闭，如果内部接收 ESC 则关闭内部，Qt6 已修复
            // 安卓返回内外会同时触发 close，Qt6 已修复
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
                            qc2_pop.close()
                        }
                    }
                }
                onClosed: {
                    console.log("Dialog sub close")
                }
            }
        }
        onClosed: {
            console.log("Dialog pop close")
        }
    }
    // Qt6 只能用他的子类了，不能直接用来自定义弹窗
    QD1.Dialog {
        id: qd1_pop
        // 默认 WindowModal
        // modality: Qt.NonModal
        // 默认的 contentItem 会处理 ESC 关闭窗口
        // 而且不会判断 modality 和 standardButtons 设置
        // 但是安卓上的返回键没有处理
        contentItem: Rectangle {
            // 给 Dialog 设置宽高在安卓有问题，所以设置 contentItem 的
            width: 200
            height: 200
            color: "green"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    qd1_pop.close()
                }
            }
        }
    }
    // 安卓侧滑返回按键后会触发 close，还导致当前 Window 的点击区域永久失效
    // 安卓侧滑Home按键后不会触发 close，还会导致当前 Window 的点击区域失效一次
    // 因为上次的 touch press 还没 release，Qt6 已修复
    MouseArea {
        z: -1
        anchors.fill: parent
        onClicked: {
            console.log("Window root clicked")
        }
    }
    onClosing: (event)=>{
                   console.log("Window root closing")
                   event.accepted = false
               }
    // 叠加 Window，安卓返回会依次处理 close
    // 没有模态阴影和点外部自动关闭的效果，但是可以自定义
    Window {
        id: win_pop
        width: 200
        height: 200
        // 默认 WindowModal
        // 模态设置不影响安卓返回触发
        modality: Qt.NonModal
        // 安卓上 Window 默认全屏，所以用透明背景的方式来显示，但是不能点击外层组件
        color: "#88282828"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("Window pop clicked")
            }
        }
        Rectangle {
            anchors.centerIn: parent
            width: 200
            height: 200
            color: "blue"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // hide 在安卓上界面会残留，Qt6 已修复
                    // win_pop.hide()
                    win_sub.show()
                }
            }
            Window {
                id: win_sub
                width: 200
                height: 200
                modality: Qt.NonModal
                color: "#88282828"
                Rectangle {
                    anchors.centerIn: parent
                    width: 100
                    height: 100
                    color: "pink"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            win_sub.close()
                            win_pop.close()
                        }
                    }
                }
                onClosing: (event)=>{
                               console.log("Window sub closing")
                           }
            }
        }
        onClosing: (event)=>{
                       console.log("Window pop closing")
                   }
    }
}
