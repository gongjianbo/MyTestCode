import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Qml Dynamic Plugin")

    menuBar: MenuBar {
        Menu {
            id: window_menu
            title: "Window"
        }
    }

    Column {
        spacing: 12
        x: 12
        y: 12
        Button {
            id: btn
            text: "Send"
            onClicked: {
                root.sendMessage(edit.text)
            }
        }
        TextField {
            id: edit
            text: "I'm Main"
            selectByMouse: true
        }
        Row {
            id: row
            spacing: 12
            MyRect{}
        }
    }

    Component {
        id: action_comp
        Action {}
    }

    Component.onCompleted: {
        for (let index in itemUrls)
        {
            //console.log(itemUrls[index])
            //QQmlComponent::PreferSynchronous首选同步
            //Component.Asynchronous同步
            let comp = Qt.createComponent(itemUrls[index], Component.PreferSynchronous)
            if (comp && comp.status === Component.Ready) {
                let item = comp.createObject(row)
            } else {
                comp.statusChanged.connect(function(){
                    if (comp.status === Component.Ready) {
                        let item = comp.createObject(row)
                    }
                });
            }
        }

        for (let index2 in windowUrls)
        {
            //console.log(windowUrls[index2])
            //QQmlComponent::PreferSynchronous首选同步
            //Component.Asynchronous同步
            let comp2 = Qt.createComponent(windowUrls[index2], Component.PreferSynchronous)
            if (comp2 && comp2.status === Component.Ready) {
                let item2 = comp2.createObject(root)
                let action2 = action_comp.createObject(window_menu)
                action2.text = item2.objectName
                action2.triggered.connect(function(){ popWindow(item2) })
                window_menu.addAction(action2)
            } else {
                comp2.statusChanged.connect(function(){
                    if (comp2.status === Component.Ready) {
                        let item2 = comp2.createObject(root)
                        let action2 = action_comp.createObject(window_menu)
                        action2.text = item2.objectName
                        action2.triggered.connect(function(){ popWindow(item2) })
                        window_menu.addAction(action2)
                    }
                });
            }
        }
    }

    signal sendMessage(string message)
    function recvMessage(message)
    {
        edit.text = message
    }
    property Window curWindow: null
    function popWindow(subWindow)
    {
        if (curWindow === subWindow) {
            curWindow.show()
            return
        }

        if (curWindow) {
            curWindow.hide()
            root.sendMessage.disconnect(curWindow.recvMessage)
            curWindow.sendMessage.disconnect(root.recvMessage)
        }
        //除了信号槽还可以设置回调函数
        curWindow = subWindow
        root.sendMessage.connect(curWindow.recvMessage)
        curWindow.sendMessage.connect(root.recvMessage)
        curWindow.show()
    }
}
