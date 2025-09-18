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

    selectionModel: ItemSelectionModel {
        id: tree_selection
    }

    // 行样式
    delegate: TreeViewDelegate {
        id: tree_del
        readonly property real _padding: 5
        readonly property real szHeight: contentItem.implicitHeight * 2

        implicitWidth: _padding + contentItem.x + contentItem.implicitWidth + _padding
        implicitHeight: szHeight

        background: Rectangle { // Background rectangle enabled to show the alternative row colors
            id: background
            anchors.fill: parent
            color: {
                if (tree_del.model.row === tree_del.treeView.currentRow) {
                    return "lightBlue"
                } else {
                    // 隔行变色
                    if (tree_del.treeView.alternatingRows && tree_del.model.row % 2 !== 0) {
                        return "#F8F8F8"
                    } else {
                        return "#F0F0F0"
                    }
                }
            }
            Rectangle { // The selection indicator shown on the left side of the highlighted row
                width: tree_del._padding
                height: parent.height
                visible: !tree_del.model.column
                color: {
                    if (tree_del.model.row === tree_del.treeView.currentRow) {
                        return (Application.styleHints.colorScheme === Qt.Light) ?
                                    Qt.darker(palette.highlight, 1.25) :
                                    Qt.lighter(palette.highlight, 2.)
                    } else {
                        return "transparent"
                    }
                }
            }
        }

        // 展开收起按钮
        indicator: Item {
            x: tree_del._padding + tree_del.depth * tree_del.indentation
            implicitWidth: tree_del.szHeight
            implicitHeight: tree_del.szHeight
            visible: tree_del.isTreeNode && tree_del.hasChildren
            rotation: tree_del.expanded ? 90 : 0
            // TreeViewDelegate内部自动给indicator创建了TapHandler
            // TapHandler {
            //     onSingleTapped: {
            //         let index = tree_del.treeView.index(tree_del.model.row, tree_del.model.column)
            //         tree_del.treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
            //         tree_del.treeView.toggleExpanded(tree_del.model.row)
            //     }
            // }
            Text {
                anchors.centerIn: parent
                text: "▶"
            }
        }

        // 文本内容，会根据model的行列取值
        contentItem: Label {
            x: tree_del._padding + (tree_del.depth + 1 * tree_del.indentation)
            width: parent.width - tree_del._padding - x
            text: tree_del.model.display
            elide: Text.ElideRight
        }
    }
}
