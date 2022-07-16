import QtQuick
import QtQuick.Controls
import MyModel

TreeView {
    id: control

    model: MyTreeModel {
        id: tree_model
        Component.onCompleted: {
            tree_model.resetItems();
        }
    }

    //可以用预定义的TreeViewDelegate样式，也可以自定义
    //delegate: TreeViewDelegate { }
    delegate: Item {
        id: root

        implicitWidth: padding + label.x + label.implicitWidth + padding
        implicitHeight: label.implicitHeight * 1.5

        readonly property real indent: 20
        readonly property real padding: 5

        //标记为required的属性将由TreeView填充，与附加属性类似。
        //delegate间接通知TreeView它应该负责为它们分配值。可以将以下必需属性添加到delegate：
        //
        //指向包含delegate Item的TreeView
        required property TreeView treeView
        //项目是否代表树中的一个节点
        //视图中只有一列将用于绘制树，因此，只有该列中的delegate Item项才会将此属性设置为true。
        //树中的节点通常应根据其缩进，如果是则depth显示指示符。
        //其他列中的delegate Item将将此属性设置为，并将显示模型中其余列的数据（通常不缩进）。
        required property bool isTreeNode
        //绘制的model Item是否在视图中展开
        required property bool expanded
        //绘制的model Item是否在模型中有子项
        required property int hasChildren
        //包含delegate绘制的model Item的深度。model Item的深度与其在模型中的祖先数量相同
        required property int depth

        //鼠标点击
        TapHandler {
            onTapped: treeView.toggleExpanded(row)
        }

        Text {
            id: indicator
            visible: root.isTreeNode && root.hasChildren
            x: padding + (root.depth * root.indent)
            text: root.expanded ? "▼" : "▶"
        }

        Text {
            id: label
            x: padding + (root.isTreeNode ? (root.depth + 1) * root.indent : 0)
            width: root.width - root.padding - x
            clip: true
            text: model.display
        }
    }
}
