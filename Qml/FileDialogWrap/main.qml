import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("FileDialog")

    Row {
        anchors.centerIn: parent
        spacing: 12
        Button {
            text: "file"
            onClicked: file.open()
        }
        Button {
            text: "files"
            onClicked: files.open()
        }
        Button {
            text: "dir"
            onClicked: dir.open()
        }
        Button {
            text: "save"
            onClicked: {
                //设置打开时的路径和文件名
                save.selectUrl(save.desktopUrl() + "/save.txt")
                save.open()
            }
        }
    }

    FileDialog {
        id: file
        mode: FileDialog.SelectFile
        onAccepted: {
            console.log('file accepted', file.currentUrl)
        }
    }
    FileDialog {
        id: files
        mode: FileDialog.SelectFiles
        onAccepted: {
            console.log('files accepted', files.currentUrls)
        }
    }
    FileDialog {
        id: dir
        mode: FileDialog.SelectDir
        onAccepted: {
            console.log('dir accepted', dir.currentUrl)
        }
    }
    FileDialog {
        id: save
        mode: FileDialog.SaveFile
        onAccepted: {
            console.log('save accepted', save.currentUrl)
        }
    }
}
