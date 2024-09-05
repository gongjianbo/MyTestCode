import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo 1.0

Window {
    id: my_window
    width: 640
    height: 480
    visible: true
    title: qsTr("Grab Image")
    color: "orange"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // C++ 自定义 Item 用来显示 QImage
        ImageView {
            id: image_view
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Column {
            spacing: 10
            Button {
                id: btn_window
                opacity: 0.5
                text: "Grab Window"
                onClicked: {
                    // Window 没有提供 QML 的接口
                    image_view.grabWindow(my_window)
                    // 如果用 contentItem 来截取则获取不到 Window 的 color，是黑色背景
                    // my_window.contentItem.grabToImage(function(result){
                    //     image_view.loadImage(result.image)
                    // })
                }
            }
            Button {
                id: btn_item
                opacity: 0.5
                text: "Grab Item"
                onClicked: {
                    // grabToImage 回调函数参数 ItemGrabResult
                    // 属性: image 和 url
                    // 方法: bool saveToFile(fileName)
                    btn_item.grabToImage(function(result){
                        // 获取到的 Image 没有透明度
                        image_view.loadImage(result.image)
                    })
                }
            }
            Button {
                id: btn_dialog
                opacity: 0.5
                text: "Grab Dialog"
                onClicked: {
                    // 没显示的话，会报错:
                    // Dialog.qml: QML QQuickPopupItem: grabToImage: item is not attached to a window
                    my_dialog.open()
                    // QuickDialog/Popup 不是继承自 Item
                    // 但是内部有一个 popupItem 根节点，没有导出为 QML 属性
                    // 在 QML 中可以通过 contentItem/background 的 parent 来间接拿到这个 Item
                    my_dialog.background.parent.grabToImage(function(result){
                        image_view.loadImage(result.image)
                    })
                }
            }
        }
    }

    Dialog {
        id: my_dialog
        opacity: 0.5
        width: 300
        height: 300
        contentItem: Rectangle {
            color: "red"
            border.color: "green"
        }
    }
}
