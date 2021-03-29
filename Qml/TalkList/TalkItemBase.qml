import QtQuick 2.12
import QtQuick.Layouts 1.12

//信息delegate的基类
//龚建波 2021-3-29
Item {
    id: control

    width: parent.ListView.view.width
    height: Math.max(profileHeight,
                     content_item.height)

    //profile picture头像
    property int profileHeight: 48
    property int messageHeight: 48

    property int leftWidth: 110
    property int rightWidth: 110
    property int contentWidth: Math.max(10,control.width-control.leftWidth-control.rightWidth)
    property bool isUser: (model.user===model.sender)
    property color messageBgColor: control.isUser?"#98E892":"#FFFFFF"

    default property alias contentItems: content_item.children

    Item{
        id: left_item
        height: control.height
        width: control.leftWidth
        //目前头像为固定，动态值可以放model
        Image {
            visible: !control.isUser
            width: control.profileHeight
            height: control.profileHeight
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/Image/profile_48_gray.png"
            //fillMode: Image.PreserveAspectFit
        }
    }

    Column{
        id: content_item
        x: control.leftWidth
        width: control.contentWidth
        spacing: 6
        //layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
    }
    Item {
        id: right_item
        height: control.height
        width: control.rightWidth
        anchors.right: parent.right
        //头像没必要放两个，可以动态锚定
        //但是区分开后可以写一些差异化的操作
        Image {
            visible: control.isUser
            width: control.profileHeight
            height: control.profileHeight
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/Image/profile_48_gray.png"
            //fillMode: Image.PreserveAspectFit
        }
    }

    //Component.onCompleted: {
    //    console.log('init',index);
    //}
    //Component.onDestruction: {
    //    console.log('free',index);
    //}
}
