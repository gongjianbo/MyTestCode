import QtQuick 2.12
import QtQuick.Window 2.12

//演示ListView的拖拽移动
Window {
    width: 360
    height: 520
    visible: true
    title: qsTr("ListView Move")

    ListModel{
        id: list_model
        Component.onCompleted: {
            for(var i=0;i<50;i++)
                list_model.append({"value": i});
        }
    }

    ListView{
        id: list_view
        anchors.fill: parent
        anchors.margins: 20
        model: list_model
        clip: true
        //interactive:false
        //当前项
        move: Transition {
            NumberAnimation { property: "y"; duration: 200 }
        }
        //被交换的项
        moveDisplaced: Transition {
            NumberAnimation { property: "y"; duration: 200 }
        }
        delegate: Rectangle{
            id: list_item
            width: ListView.view.width
            height: 40
            border.color: "white"
            color: mouse_area.pressed?"black":"gray"
            z: mouse_area.pressed?10:1
            Text {
                anchors.centerIn: parent
                text: "row:"+index+" value:"+value
                color: "white"
                font.pixelSize: 20
            }
            MouseArea {
                id: mouse_area
                anchors.fill: parent
                //防止被view窃取事件
                preventStealing: true
                onPositionChanged: {
                    //item在view位置+鼠标在item中位置，判断是否拖动到了其他item上，是的话就交换两者
                    var other_index = list_view.indexAt(mouse_area.mouseX + list_item.x, mouse_area.mouseY + list_item.y);
                    if (other_index !== -1 && pressed) {
                        if (index !== other_index) {
                            list_model.move(index, other_index, 1);
                        }
                    }
                }
            }
        }
    }
}
