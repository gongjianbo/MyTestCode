import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml.Models 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

Window {
    width: 720
    height: 480
    visible: true
    title: qsTr("TreeView")

    SimpleTreeModel {
        id: tree_model
        //ItemSelectionModel resetModel信号block了信号
        onModelAboutToBeReset: {
            tree_view.clearSelect()
        }
    }

    ItemSelectionModel {
        id: tree_selection
        model: tree_model
        onCurrentIndexChanged: {
            console.log('selection currentIndex', currentIndex)
        }
        onCurrentChanged: function(current, previous){
            console.log('selection current', current, previous)
        }
    }

    BasicTreeView {
        id: tree_view
        anchors.fill: parent
        anchors.margins: 12
        anchors.bottomMargin: 60
        model: tree_model
        selection: tree_selection
        onCurrentIndexChanged: {
            console.log('view currentIndex', currentIndex, __currentRow)
        }
        //on__CurrentRowChanged: {
        //    console.log('view currentRow', currentIndex, __currentRow)
        //}
        //on__CurrentRowItemChanged: {
        //    console.log('view currentItem', currentIndex, __currentRow)
        //}
        style: BasicTreeViewStyle {

        }
        BasicTreeViewColumn {
            title: "TreeColumn"
            role: "display"
            resizable: true
        }
    }

    Row {
        x: 12
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        spacing: 12
        Button {
            text: "append sub"
            onClicked: {
                tree_model.appendNode(tree_selection.currentIndex, "Sub")
                tree_view.forceActiveFocus()
            }
        }
        Button {
            text: "remove node"
            onClicked: {
                tree_model.removeNode(tree_selection.currentIndex)
                tree_view.forceActiveFocus()
            }
        }
        Button {
            text: "reset"
            onClicked: {
                //tree_view.model = null
                tree_model.reset()
                //tree_view.model = tree_model
                var index = tree_model.index(0, 0)
                //为了同步，reset后设置index
                tree_view.selectIndex(index)
            }
        }
        Button {
            text: "clear"
            onClicked: {
                tree_model.clear()
            }
        }
        Button {
            text: "select (1,0)"
            onClicked: {
                var index = tree_model.index(1, 0, tree_model.index(1, 0))
                tree_view.expandIndex(index)
                tree_view.selectIndex(index)
            }
        }
        Button {
            text: "log"
            onClicked: {
                //1.选中sub后收起，此时view和selection的current不一致
                //selection保持被收起的的index，view变为被收起的下一行
                //没有设置selection则styleData高亮的view选中的，有则高亮的selection选中的
                //但是key按键导航是用的view的
                //2.重新赋值model后view index重置，selection会保留原来的
                //model reset后view index保持，selction重置但在某些情况下没刷新
                //3.currentIndex绑定的逗号表达式：(modelAdaptor.updateCount, modelAdaptor.mapRowToModelIndex(__currentRow))
                //treeview收起，如果在当前项之前，由于currentIndex先被adaptor counter触发一次，
                //此时__currentRow还没同步导致计算的index异常
                //4.selection的setCurrentIndex不会触发view的index刷新，要自己同步设置两个
                //但是treeview的用的listview的row，adaptor index转row的函数没有导出到qml
                //同样key按键导航是用的view的
                //5.ItemSelectionModel reset是block了信号的，所以reset有时候currentIndexChanged没触发
                //可以reset前手动重置下currentIndex
                console.log('log view current', tree_view.currentIndex)
                console.log('log selection current', tree_selection.currentIndex)
            }
        }
    }
}
