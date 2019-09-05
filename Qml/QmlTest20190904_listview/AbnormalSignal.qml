import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Shapes 1.12
import MyModel 1.0

//异常信号追踪
//左侧为时间点相关的table
//右侧为频段相关的list

Item{
    id:item_root

    property int headerWidth: 100
    property int itemSpace: 2
    property int itemHeight: 15
    property double itemWidth: (table_view.width-headerWidth-itemSpace)/60-itemSpace


    ListView{
        id:table_view

        anchors.fill: parent
        anchors.rightMargin: list_view.width+5
        clip: true

        spacing: 1

        model:AbnormalListModel{
            id:table_model
        }

        //滚动条会遮挡 hover导致tip不显示
        ScrollBar.horizontal: ScrollBar {
            enabled: false
        }
        ScrollBar.vertical: ScrollBar {
            enabled: false
        }


        delegate: Row{
            id:table_row
            z:0.1
            spacing: itemSpace
            Rectangle{
                    width: headerWidth
                    height: itemHeight
                    color: "gray"
                    Text{
                        anchors.centerIn: parent
                        text:  hour //"1992"
                        color: "white"
                    }
            }

            Repeater{
                model:60
                Rectangle{
                    id:table_item
                    width: itemWidth
                    height: itemHeight
                    property int abnormal: table_model.abnormal(table_row.index,index)
                    color: {
                        switch(table_item.abnormal)
                        {
                            //case -1: return "gray";
                        case 0: return "green";
                        case 1: return "pink";
                        default:break;
                        }
                        return "gray";
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: table_item.abnormal>0?true:false
                        onEntered: table_itemtip.visible=true
                        onExited: table_itemtip.visible=false
                    }
                    //每个都有tip的话太卡了
                    ToolTip{
                        id: table_itemtip
                        text: {
                            switch(table_item.abnormal)
                            {
                                //case -1: return "none";
                            case 0: return "success";
                            case 1: return "error 1";
                            default:break;
                            }
                            return "none";
                        }
                    }
                }
            }
        }

        headerPositioning: ListView.OverlayHeader
        header:Rectangle{
            z:1
            height: 30
            width: table_view.width
            color: Qt.darker("gray")
            Row{
                anchors.verticalCenter: parent.verticalCenter
                spacing: itemSpace
                Item{
                    width: headerWidth
                    height: itemHeight
                }

                Repeater{
                    model:60
                    Text{
                        width: itemWidth
                        height: itemHeight
                        color: "white"
                        text: index%5?"":index
                    }
                }
            }
        }
        footerPositioning:ListView.OverlayFooter
        footer: Rectangle{
            height: 10
            width: table_view.width
            color: Qt.darker("gray")
        }
    }

    ListView{
        id:list_view
        width: 160
        anchors.right: parent.right
        height: parent.height
        clip: true
        currentIndex: table_model.currentIndex

        //弹性互动，但是这样好像连滚动也没了
        //interactive: false
        spacing: 1
        //focus: true

        model:table_model.groupList()

        ScrollBar.horizontal: ScrollBar {}
        ScrollBar.vertical: ScrollBar {}

        delegate:Rectangle{
            z:0.1
            height: 30
            width: list_view.width
            color: "transparent"
            border.width: 1
            border.color: "gray"
            Text{
                anchors.fill: parent
                color: parent.ListView.isCurrentItem?"white":"gray"
                text: modelData
                font{
                    family: "SimSun"
                    pixelSize: 14
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea{
                anchors.fill: parent
                //hoverEnabled: true
                //onEntered: parent.ListView.view.currentIndex = index
                onClicked: table_model.currentIndex = index
            }
        }
        highlight:Rectangle{
            height: 30
            width: list_view.width
            color: "gray"
        }

        headerPositioning: ListView.OverlayHeader //PullBackHeader
        header: Rectangle{
            height: 30
            width: list_view.width
            color: Qt.darker("gray")

            Text {
                anchors.centerIn: parent
                text: "频率（MHz）"
                color: "white"
                font{
                    family: "SimSun"
                    pixelSize: 14
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Button{
                id:list_menu
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 5
                width: 20
                height: 10
                background: Shape{
                    //多重采样抗锯齿
                    //layer.enabled: true
                    //layer.samples: 8
                    //平滑处理
                    //smooth: true
                    //默认抗锯齿
                    //antialiasing: true
                    ShapePath{
                        //color默认为白色
                        //strokeWidth: 1
                        strokeColor: "transparent"
                        fillColor: list_menu.hovered?"cyan":"white"
                        fillRule: ShapePath.WindingFill
                        startX: 0
                        startY: 0
                        PathLine{x:20;y:0}
                        PathLine{x:10;y:10}
                        PathLine{x:0;y:0}
                    }
                }
                onClicked: {
                    console.log("弹一个范围选择对话框出来")
                }
            }
        }
        footerPositioning:ListView.OverlayFooter
        footer: Rectangle{
            height: 10
            width: list_view.width
            color: Qt.darker("gray")
        }
    }
}

