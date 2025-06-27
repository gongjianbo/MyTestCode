import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("FileDialog")

    Row {
        anchors.centerIn: parent
        spacing: 12
        Button {
            text: "file"
            onClicked: {
                // 选择文件
                let dlg = FileDialogManager.selectFile()
                // 选择过滤
                dlg.nameFilters = ["Image File (*.jpg; *.bmp; *.png;)", "All Files (*.*)"]
                dlg.onAccepted.connect(function() {
                    console.log("select file", dlg.currentUrl)
                })
                dlg.open()
            }
        }
        Button {
            text: "files"
            onClicked: {
                // 多选文件
                let dlg = FileDialogManager.selectFiles()
                dlg.onAccepted.connect(function() {
                    console.log("select files", dlg.currentUrls)
                })
                dlg.open()
            }
        }
        Button {
            text: "dir"
            onClicked: {
                // 选择目录
                let dlg = FileDialogManager.selectDir()
                dlg.onAccepted.connect(function() {
                    console.log("select dir", dlg.currentUrl)
                })
                dlg.open()
            }
        }
        Button {
            text: "save"
            onClicked: {
                // 保存文件
                let dlg = FileDialogManager.saveFile()
                // 选择过滤
                dlg.nameFilters = ["JPEG (*.jpg)", "BMP (*.bmp)", "PNG (*.png)"]
                // 选中的过滤类型
                dlg.selectedNameFilter = dlg.nameFilters[1]
                let path_str = dlg.lastPath() + "/new_file"
                // 预置文件路径
                dlg.selectPath(path_str)
                dlg.onAccepted.connect(function() {
                    console.log("save file", dlg.currentUrl)
                })
                dlg.open()
            }
        }
    }
}
