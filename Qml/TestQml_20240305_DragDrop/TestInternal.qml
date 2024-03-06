import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Item {
    width: 640
    height: 480

    GridLayout {
        anchors.fill: parent
        anchors.margins: 12
        rowSpacing: 12
        columnSpacing: 12
        rows: 2
        columns: 2
        Repeater {
            model: 3
            delegate: Rectangle {
                id: item
                Layout.fillWidth: true
                Layout.fillHeight: true
                border.color: "gray"
                Image {
                    id: image
                    width: item.width
                    height: item.height
                    z: 100
                    source: "qrc:/flower.png"
                    fillMode: Image.PreserveAspectFit

                    // 不设置active也能拖动，但是Drop不会响应
                    // Drag.active: mouse.drag.active
                    Drag.supportedActions: Qt.CopyAction
                    // dragType=Drag.Internal(默认)调用start()，为当前Item跟随拖动
                    // 但是DropArea的drag/drop事件拿不到mimeData内容，且drop不自动触发
                    // =Drag.Automatic调用startDrag()，为当前imageSource图片跟随拖动
                    // 拖动结束Item才会移动过去，只有imageSource是跟随的
                    // =Drag.None则自己调用start()/startDrag()/stop()
                    // Drag.dragType: Drag.Automatic
                    // Drag.mimeData: { "text/plain": "gongjianbo"}
                    // 和DropArea的keys匹配
                    // Drag.keys: ["red"]
                    // 和鼠标的相对位置
                    Drag.hotSpot.x: width / 2
                    Drag.hotSpot.y: height / 2
                    // imageSource配合dragType=Drag.Automatic使用，默认是原图大小
                    // Drag.imageSource: image.source
                    // 如果start函数()启动，则不会触发started和finished信号
                    Drag.onDragStarted: {
                        console.log("item drag start")
                    }
                    Drag.onDragFinished: {
                        console.log("item drag finish")
                    }
                    Text {
                        text: "layer"
                    }
                }
                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    property point pressPos: Qt.point(0, 0)
                    onPressed: pressPos = Qt.point(mouseX, mouseY)
                    // onPositionChanged: console.log(mouseX)
                    // 如果是外部放一个item来显示，按下的时候设置位置没效果，结束才更新了位置
                    // 所以直接用image做target
                    drag.target: image
                    drag.onActiveChanged: {
                        console.log("mouse area drag active", drag.active)
                        if (drag.active) {
                            image.Drag.mimeData = {
                                "text/uri-list": image.source,
                                "text/plain": "copy image" + model.index
                            }
                            image.parent = item.Window.window.contentItem
                            image.Drag.active = true
                        } else {
                            image.Drag.drop()
                            image.Drag.active = false
                            image.parent = item
                            image.x = 0
                            image.y = 0
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: drop_area.containsDrag ? "cyan" : "gray"
            DropArea {
                id: drop_area
                anchors.fill: parent
                onEntered: {
                    console.log("drop enter", drag.source, drag.text)
                }
                onDropped: {
                    console.log("drop drop", drag.source, drop.text)
                }
                onExited: {
                    console.log("drop exit")
                }
            }
        }
    }
}
