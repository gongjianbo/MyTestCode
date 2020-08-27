import QtQuick 2.9
import QtQuick.Window 2.9
import QtQuick.Controls 2.9

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("GongJianBo 1992")

    Column{
        spacing: 10
        Button{
            text: "Quit"
            //调用Qt.quit不会进onClosing
            onClicked: Qt.quit()
        }
        Button{
            text: "Exit"
            //调用Qt.exit不会进onClosing
            onClicked: Qt.exit(0)
        }
        Button{
            text: "Close"
            //调用Window.close会进onClosing
            onClicked: root.close()
        }
    }

    //可能是qmltype信息不全，有M16警告，这里屏蔽下
    //@disable-check M16
    onClosing: function(closeevent){
        //点击标题栏关闭按钮，弹一个退出提示框
        warning.open()
        //CloseEvent的accepted设置为false就能忽略该事件
        closeevent.accepted = false
    }

    Popup{
        id:warning
        anchors.centerIn: parent
        Column{
            spacing: 10
            Text {
                text: qsTr("确认退出？")
            }
            Button{
                text: "是"
                onClicked: Qt.quit()
            }
            Button{
                text: "否"
                onClicked: warning.close()
            }
        }
    }
}
