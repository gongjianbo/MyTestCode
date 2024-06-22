import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Qml Toast")

    // 也可以注册为单例使用
    ToastManager {
        id: toast_manager
    }

    Row {
        anchors.centerIn: parent
        spacing: 10
        TextField {
            id: text_filed
            text: "GongJianBo"
        }
        Button {
            text: "Pop"
            onClicked: {
                toast_manager.showToast(text_filed.text)
            }
        }
    }
}
