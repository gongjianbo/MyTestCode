import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Qml Struct")

    property var callback

    Column {
        anchors.centerIn: parent
        spacing: 12
        Button {
            text: "Data"
            onClicked: {
                // 测试自定义类型
                var data = CppTool.createData()
                data.name = "qml set name"
                console.log("typeof", typeof data)
                // 直接回调
                CppTool.setData(data)
                // QML中持有引用，再回调
                callback = function() {
                    CppTool.setData(data)
                }
            }
        }
        Button {
            text: "Meta"
            onClicked: {
                // 测试Qt自带类型
                var data = CppTool.createMeta()
                console.log("typeof", typeof data)
                // 直接回调
                CppTool.setMeta(data)
                // QML中持有引用，再回调
                callback = function() {
                    CppTool.setMeta(data)
                }
            }
        }
        Button {
            text: "Callback"
            onClicked: {
                // QImage/QPixmap这种稀缺资源类型，QML不会持有其引用，生成之后可以立即使用但没法用变量暂存
                if (callback) {
                    callback()
                    callback = null
                }
            }
        }
        Button {
            text: "QML GC"
            onClicked: {
                console.log("gc")
                gc()
            }
        }
    }

}
