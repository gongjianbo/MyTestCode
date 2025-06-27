pragma Singleton
import QtQuick 2.15
import GongJianBo 1.0

// 文件对话框管理
// 龚建波 2025-04-16
Item {
    id: control

    // 存放对象的引用，避免被释放
    property var itemArr: []
    // 创建窗口对象
    function createItem(comp, parent) {
        // 如果指定了parent，自动成为transientParent
        let obj = comp.createObject(parent)
        console.log("[construct]", obj)
        obj.onClosing.connect(function() {
            console.log("[destroy]", obj)
            let idx = itemArr.indexOf(obj)
            if (idx >= 0) {
                itemArr.slice(idx, 1)
            }
            obj.destroy()
            obj = null
            // 主要用于清理传递的参数
            gc()
        })
        itemArr.push(obj)
        return obj
    }

    // 文件对话框
    Component {
        id: file_component
        BasicFileDialog { }
    }
    // 选择文件
    function selectFile(parent = null) {
        let dlg = createItem(file_component, parent)
        dlg.mode = BasicFileDialog.SelectFile
        return dlg
    }
    // 多选文件
    function selectFiles(parent = null) {
        let dlg = createItem(file_component, parent)
        dlg.mode = BasicFileDialog.SelectFiles
        return dlg
    }
    // 选择目录
    function selectDir(parent = null) {
        let dlg = createItem(file_component, parent)
        dlg.mode = BasicFileDialog.SelectDir
        return dlg
    }
    // 保存文件
    function saveFile(parent = null) {
        let dlg = createItem(file_component, parent)
        dlg.mode = BasicFileDialog.SaveFile
        return dlg
    }
}
