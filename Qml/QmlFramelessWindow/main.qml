import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

//演示FramelessHelper的使用
Window {
    id: root
    width: 640
    height: 480
    minimumHeight: 200
    minimumWidth: 500
    visible: true
    title: qsTr("Qml 简易无边框 (by 龚建波)")
    flags: Qt.Window
           |Qt.FramelessWindowHint
           |Qt.WindowMinMaxButtonsHint

    FramelessHelper{
        window: root
        title: title_area
        borderWidth: 6
    }

    //边框
    Rectangle{
        anchors.fill: parent
        border.color: "gray"
        border.width: 6
    }

    //标题栏
    MouseArea{
        id: title_area
        height: 50
        width: root.width
        Rectangle{
            anchors.fill: parent
            color: "darkCyan"
            opacity: 0.5
        }

        Text{
            anchors{
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 20
            }
            font.pixelSize: 20
            color: "white"
            text: root.title
        }

        Row{
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 10
            spacing: 10

            Button{
                width: 70
                height: 30
                text: "min"
                onClicked: root.showMinimized()
            }
            Button{
                width: 70
                height: 30
                text: "max"
                onClicked: {
                    if(root.visibility==Window.Maximized)
                        root.showNormal()
                    else
                        root.showMaximized()
                }
            }
            Button{
                width: 70
                height: 30
                text: "close"
                onClicked: Qt.quit()
            }
        }
    }

    ListView{
        anchors.fill: parent
        anchors.margins: 20
        anchors.topMargin: 70
        clip: true
        model: 50
        spacing: 5
        delegate: Rectangle{
            height: 40
            width: ListView.view.width
            border.color: "gray"
            Text {
                anchors.centerIn: parent
                text: index
            }
        }
    }
}
