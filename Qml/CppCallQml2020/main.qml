import QtQuick 2.9

Item{
    id: root
    width: 250
    height: 250
    //自定义属性  --cpp可以访问
    property string msg: "GongJianBo1992"
    //自定义信号  --可以触发cpp槽函数
    signal qmlSendMsg(string arg1,string arg2)

    Rectangle {
        anchors.fill: parent
        color: "green"
        objectName: "rect" //用于cpp查找对象
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("qml 点击鼠标, 发送信号 qmlSendMsg")
            root.qmlSendMsg(root.msg,"myarg2")
        }
    }

    onHeightChanged: console.log("qml height changed")
    onWidthChanged: console.log("qml width changed")

    //QML中的方法可以被cpp调用，也可以作为槽函数
    function qml_method(val_arg){
        console.log("qml method runing",val_arg,"return ok")
        return "ok"
    }
    //注意槽函数参数为var类型
    function qmlRecvMsg(arg1,arg2){
        console.log("qml slot runing",arg1,arg2)
    }
}
