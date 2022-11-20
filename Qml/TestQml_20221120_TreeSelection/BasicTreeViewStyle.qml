import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.15

//TreeView样式，修改自BasicTableViewStyle
//文档参见TreeView文档
//龚建波 2022-11-20
ScrollViewStyle {
    id: root

    readonly property BasicTreeView control: __control
    //获取对应行图标
    property var depthIcon: function(depth) {
        return "";
    }
    //行高
    property int rowHeight: 24
    padding {
        top: 0
        left: 0
        right: 0
        bottom: 0
    }
    //整个区域，在bgColor之下
    frame: Rectangle {
        color: "#E8E8E8"
    }
    //滚动条宽度
    property int barWidth: 15
    //滚动条背景
    scrollBarBackground: Rectangle {
        implicitWidth: barWidth
        implicitHeight: barWidth
        color: "#F0F0F0"
    }
    //滚动条滑条
    handle: Item  {
        implicitWidth: barWidth
        implicitHeight: barWidth
        //他这个位置有点问题，加1px纠正
        Rectangle {
            opacity: 0.99999
            color: (styleData.hovered || styleData.pressed) ? "#A6A6A6" : "#CDCDCD"
            anchors {
                fill: parent
                leftMargin: styleData.horizontal ? 1 : 0
                rightMargin: styleData.horizontal ? 1 : 0
                topMargin: styleData.horizontal ? 0 : 1
                bottomMargin: styleData.horizontal ? 0 : 1
            }
        }
    }
    //横竖滚动条交界处
    corner: null
    //往左或上减少
    decrementControl: null
    //往右或下增加
    incrementControl: null

    //【】TreeViewStyle中的属性
    //readonly property TreeView control: __control
    //控制缩进
    property int indentation: 20
    //展开图标，有子项时才显示
    property Component branchDelegate: Item {
        width: indentation
        height: indentation
        Image {
            scale: 0.6
            anchors.centerIn: parent
            rotation: styleData.isExpanded ? 90 : 0
            //这个图标是controls2的，所以需要调用之前有导入controls2
            source: "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/arrow-indicator.png"
        }
    }
    //__branchDelegate: branchDelegate
    //__indentation: indentation

    //【】BasicTableViewStyle中的属性
    //readonly property BasicTableView control: __control
    property color textColor: SystemPaletteSingleton.text(control.enabled)
    property color backgroundColor: control.backgroundVisible ? SystemPaletteSingleton.base(control.enabled) : "transparent"
    property color alternateBackgroundColor: "#f5f5f5"
    property color highlightedTextColor: "white"
    property bool activateItemOnSingleClick: false
    //padding.top: control.headerVisible ? 0 : 1

    //表头
    property Component headerDelegate: Rectangle {
        height: rowHeight
        color: "#F0F0F0"
        Text {
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            leftPadding: 6
            text: styleData.value
        }
    }
    //整个行
    property Component rowDelegate: Rectangle {
        height: rowHeight
        //隔行变色
        color: styleData.alternate ? "#E8E8E8" : "#E0E0E0"
    }
    //文本部分
    //因为branch有子项才显示，所以画虚线可以在item中绘制
    property Component itemDelegate: Item {
        id: item_del
        height: rowHeight
        Image {
            id: item_icon
            anchors.verticalCenter: parent.verticalCenter
            source: depthIcon(styleData.depth)
        }
        Rectangle {
            x: item_icon.width + 2
            visible: styleData.selected
            height: parent.height
            width: item_text.width
            color: "#0078D7"
        }
        Text {
            id: item_text
            x: item_icon.width + 2
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            leftPadding: 6
            rightPadding: 6
            bottomPadding: 1
            text: styleData.value
            font.bold: styleData.depth === 0
            color: styleData.selected ? "white" : "black"
        }
    }
    property Component __branchDelegate: branchDelegate
    property int __indentation: indentation
}
