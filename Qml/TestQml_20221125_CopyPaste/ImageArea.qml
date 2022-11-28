import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo 1.0

Rectangle {
    id: control

    //onLoading 正在加载图片
    property bool onLoading: drop_image.status === Image.Loading
    //drop_area.containsDrag 正在拖拽
    //focus_scope.activeFocus 有焦点，可以响应按键
    border.color: onLoading
                  ? "red"
                  : drop_area.containsDrag
                    ? "orange"
                    : focus_scope.activeFocus
                      ? "cyan"
                      : "black"

    FocusScope {
        id: focus_scope
        Keys.enabled: true
        //用release的话，按键释放顺序不能保证
        Keys.onPressed: function(event) {
            if (event.modifiers & Qt.ControlModifier)
            {
                if (event.key === Qt.Key_C) {
                    //Ctrl+C复制
                    console.log("ctrl + c")
                    copy()
                } else if(event.key === Qt.Key_V) {
                    //Ctrl+V复制
                    console.log("ctrl + v")
                    paste(ClipboardTool.currentUrl())
                }
            }
        }
    }
    Image {
        id: drop_image
        anchors.fill: parent
        anchors.margins: 1
        //按比例缩放
        fillMode: Image.PreserveAspectFit
        //异步加载
        asynchronous: true
        visible: !onLoading
    }
    Text {
        anchors.centerIn: parent
        //提示正在加载
        text: "on Loading"
        visible: onLoading
    }
    DropArea {
        id: drop_area
        anchors.fill: parent
        onDropped: function(drop){
            if (onLoading) {
                console.log("on loading")
                return;
            }

            if (drop.hasText && drop.text === "copy image") {
                console.log("copy image inner")
                //内部拖拽时设置一个text标记，避免内部拖拽复制
                //这里为了方便用的text，可以自定义其他数据
                return;
            }

            console.log("drop hasUrls:", drop.hasUrls)
            if (drop.hasUrls) {
                paste(drop.urls[0])
            }
        }
    }
    Item {
        id: drag_item
        Drag.active: mouse_area.drag.active
        //imageSource是拖拽时的图标
        //这里有个问题，图片是原始大小，而且没看到有设置大小或者缩放的接口
        //可以在拖入的时候就生成一个
        //Drag.imageSource: drop_image.source
        //hotSpot是鼠标与imageSource的偏移距离
        Drag.hotSpot.x: 5
        Drag.hotSpot.y: 5
        //拖拽类型默认Drag.Internal不知道干嘛的
        Drag.dragType: Drag.Automatic
        //拖拽拷贝文件，默认好像是移动了文件
        Drag.supportedActions: Qt.CopyAction
        //mimeData是拖拽的内容
        Drag.mimeData: {
            "text/uri-list": drop_image.source,
            "text/plain": "copy image"
        }
    }
    MouseArea {
        id: mouse_area
        anchors.fill: parent
        //加载好图片后允许拖出
        drag.target: (!onLoading && drop_image.frameCount > 0) ? drag_item : null
        onClicked: {
            focus_scope.forceActiveFocus()
        }
    }

    //复制
    function copy()
    {
        if (!onLoading && drop_image.frameCount > 0)
        {
            ClipboardTool.setCurrentUrl(drop_image.source)
        }
    }

    //粘贴
    function paste(url)
    {
        //cpp类判断url指向的文件是否为可解析为图片
        if (FileTool.isImageFile(url)) {
            focus_scope.forceActiveFocus()
            console.log("paste url:", url)
            //根据需求，这里可以复制到指定目录后取新的url，可以进行格式转换
            //考虑到大文件耗时
            let cache_url = FileTool.copyImage(url)
            if (FileTool.urlValid(cache_url)) {
                drop_image.source = cache_url
            }
        }
    }
}
