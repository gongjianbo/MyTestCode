import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TableView{
    //frameVisible: false
    //backgroundVisible: false
    TableViewColumn{
        role: "r0"
        title:"序号"
    }
    TableViewColumn{
        role: "r1"
        title:"频率"
    }
    TableViewColumn{
        role: "r2"
        title:"示向度 "
    }
    TableViewColumn{
        role: "r3"
        title:"罗盘值 "
    }
    TableViewColumn{
        role: "r4"
        title:"电平"
    }
    TableViewColumn{
        role: "r5"
        title:"测向质量"
    }
    TableViewColumn{
        role: "r6"
        title:"经度"
    }
    TableViewColumn{
        role: "r7"
        title:"纬度"
    }
    TableViewColumn{
        role: "r8"
        title:"保存"
    }
    model: ListModel{
        ListElement{
            r0:"+"
            r1:"-"
            r2:"x"
            r3:"/"
        }
    }
}

/*
    Rectangle{
        id:table_view
        color: "#bccdbc"
        anchors.fill: parent
        anchors.rightMargin: list_view.width+5

        TableView{
            anchors.fill: parent
            anchors.topMargin: 16
            anchors.leftMargin: 50
            rowSpacing: 1
            columnSpacing: 1
            clip: true

            //去掉滚动条
            //ScrollBar.horizontal: ScrollBar {}
            //ScrollBar.vertical: ScrollBar {}

            delegate: Rectangle{
                implicitWidth: 10
                implicitHeight: 20
                color: "gray"
                MouseArea{
                    anchors.fill: parent
                    onClicked: parent.color="white"
                }
            }

            model:AbnormalSignalModel{
            }
        }
    }

*/
