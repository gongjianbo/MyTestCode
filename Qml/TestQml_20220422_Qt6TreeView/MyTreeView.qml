import QtQuick
import QtQuick.Controls
import MyModel

// qt6 qml TreeView样式自定义
// 龚建波 2025-09-19
TreeView {
    id: control
    clip: true

    model: MyTreeModel {
        id: tree_model
        Component.onCompleted: {
            tree_model.resetItems()
        }
    }

    // 选中效果
    selectionModel: ItemSelectionModel {
        id: tree_selection
    }

    // 单元格样式
    // 可以用预定义的TreeViewDelegate样式，也可以自定义
    // delegate: TreeViewDelegate { }
    delegate: Rectangle {
        id: tree_del

        // 因为TreeView是继承自TableView，delegate第1列太宽后面的列就显示不出来了
        // implicitWidth: TableView.view.width
        implicitWidth: padding + label.x + label.implicitWidth + padding
        implicitHeight: label.implicitHeight * 2
        color: {
            if (model.row === tree_del.treeView.currentRow) {
                return "lightBlue"
            } else {
                // 隔行变色，注意这个model.row
                if (tree_del.treeView.alternatingRows && model.row % 2 !== 0) {
                    return "#F8F8F8"
                } else {
                    return "#F0F0F0"
                }
            }
        }

        // 缩进
        readonly property real indent: 20
        // 左右边距
        readonly property real padding: 5

        // 标记为required的属性将由TreeView填充，与附加属性类似。
        // delegate间接通知TreeView它应该负责为它们分配值。可以将以下必需属性添加到delegate：
        //
        // 指向包含delegate Item的TreeView
        required property TreeView treeView
        // 项目是否代表树中的一个节点
        // 视图中只有一列将用于绘制树节点，因此，只有该列中的delegate Item项才会将此属性设置为true。
        // 树中的节点通常应根据其缩进，如果是则depth显示指示符。
        // 其他列中的delegate Item将将此属性设置为，并将显示模型中其余列的数据（通常不缩进）。
        required property bool isTreeNode
        // 绘制的model Item是否在视图中展开
        required property bool expanded
        // 绘制的model Item是否在模型中有子项
        required property int hasChildren
        // 包含delegate绘制的model Item的深度。model Item的深度与其在模型中的祖先数量相同
        required property int depth

        Rectangle {
            width: tree_del.padding
            height: parent.height
            // 选中项第一列高亮的色块
            visible: !model.column && (model.row === tree_del.treeView.currentRow)
            color: "darkBlue"
        }

        Text {
            id: indicator
            visible: tree_del.isTreeNode && tree_del.hasChildren
            x: tree_del.padding + (tree_del.depth * tree_del.indent)
            width: tree_del.indent
            height: tree_del.height
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: tree_del.expanded ? "▼" : "▶"
            // Rectangle { anchors.fill: parent; color: "#88FF0000" }
            // 鼠标点击展开节点
            TapHandler {
                onTapped: treeView.toggleExpanded(row)
            }
        }

        Text {
            id: label
            x: tree_del.padding + (tree_del.isTreeNode ? (tree_del.depth + 1) * tree_del.indent : 0)
            width: tree_del.width - tree_del.padding - x
            height: tree_del.height
            verticalAlignment: Text.AlignVCenter
            text: model.display
            // Rectangle { anchors.fill: parent; color: "#8800FF00" }
        }
    }
}
