import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Tools 1.0

//文档：https://doc.qt.io/qt-5/qtquick-input-focus.html
//当用户按下或释放一个按键时：
//1.Qt接受按键动作并生成一个按键事件
//2.如果QQuickWindow是active window，则按键事件传递给它
//3.按键事件由scene传递给active focus的Item(activeFocus属性)，
//  如果当前没有active focus Item则此按键事件被忽略(ignore)
// （注：如果注册了快捷键，Item收到事件顺序为ShortcutOverride-KeyPress-KeyRelease，
//  ShortcutOverride如果自己没处理交给Shortcut了，就不会触发Press，只剩Release）
//4.如果此active focus Item接受(accept)此事件，则传递结束，
//  否则一直往父级Item传递，直到达到根节点
Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("QML ShortcutKey by:GongJianbo1992")
    color: "darkGreen"
    property int counter: 0

    Shortcut {
        id: short1
        //长按重复触发
        autoRepeat: true //default
        //过滤当前Window or 整个应用
        context: Qt.WindowShortcut //default
        //快捷键
        //sequence: "left,right" //按了left再按right触发
        //sequences: ["left","right"] //按left或者right触发
        //sequences: ["return","enter"] //大小键盘回车
        sequences: ["+","delete","return","enter","space"]
        //只有一个Shortcut关注该快捷键则只触发onActivated
        //否则只触发onActivatedAmbiguously
        onActivated: {
            console.log('Shortcut 1 onActivated.',counter++)
        }
        onActivatedAmbiguously: {
            console.log('Shortcut 1 onActivatedAmbiguously.',counter++)
        }
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 2
        columnSpacing: 10
        rows: 2
        rowSpacing: 10

        Rectangle {
            id: rect1
            Layout.fillWidth: true
            Layout.fillHeight: true
            //非输入组件没法自动获得focus
            border.width: 2
            border.color: focus?"red":"gray"

            Column {
                anchors.centerIn: parent
                spacing: 10
                //编辑状态下，delete会被编辑组件处理，
                //但回车这种单行组件不处理的会被shortcut捕获
                TextField {
                    id: input1
                    width: 100
                    height: 30
                    placeholderText: "input"
                    background: Rectangle {
                        border.color: input1.activeFocus?"red":"gray"
                    }

                    //Keys在所附加的Item激活(activeFocus)时才触发
                    //被编辑组件或者shortcut捕获后，Keys不触发pressed
                    Keys.onDeletePressed: {
                        console.log('Keys delete pressed.',counter++)
                    }
                    //被编辑组件接受的都会触发pressed
                    Keys.onPressed: {
                        console.log('Keys pressed.',event.key,counter++)
                    }

                    //回车被shortcut捕获，不触发finished
                    onEditingFinished: {
                        console.log('TextFiled finished.',counter++)
                    }
                }
                //空格被shortcut捕获，触发clicked但不触发pressed
                Button {
                    width: 100
                    height: 30
                    text: "finish"
                    onPressed: {
                        console.log('Button1.1 pressed.',counter++)
                    }
                    onClicked: {
                        console.log('Button1.1 clicked.',counter++)
                    }
                }
            }
        }

        Rectangle {
            id: rect2
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.width: 2
            border.color: focus?"cyan":"white"

            Column {
                anchors.centerIn: parent
                spacing: 10
                Button {
                    width: 100
                    height: 30
                    text: "button"
                    onPressed: {
                        console.log('Button2.1 pressed.',counter++)
                    }
                    onClicked: {
                        console.log('Button2.1 clicked.',counter++)
                    }
                }
                //无论点击还是快捷键，会先触发Triggered，再触发Clicked
                //但只在点击时才触发Pressed
                Button {
                    width: 100
                    height: 30
                    text: "+"
                    action: action1
                    onPressed: {
                        console.log('Button2.2 pressed.',counter++)
                    }
                    onClicked: {
                        console.log('Button2.2 clicked.',counter++)
                    }
                    //check状态会受action影响
                    onCheckedChanged: {
                        console.log('Button2.2 checked.',checked,counter++)
                    }
                }

                MenuBar {
                    //anchors.centerIn: parent
                    Menu {
                        title: "menu"
                        Action {
                            id: action1
                            text: "+1"
                            checkable: true
                            shortcut: "+"
                            //toggle开关/激活，checkable=true时同trigger一起触发
                            onToggled: {
                                console.log('Action +1 toggled.',counter++)
                            }
                            //trigger触发
                            //无论点击还是快捷键，会先触发Triggered，再触发Clicked
                            //但只在点击时才触发Pressed
                            onTriggered: {
                                console.log('Action +1 triggered.',counter++)
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: rect3
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.width: 2
            border.color: focus_scope.activeFocus?"cyan":"white"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log('FocusScope forceActiveFocus.')
                    focus_scope.forceActiveFocus();
                    focus_item.focus = true;
                }
            }

            Shortcut {
                context: Qt.ApplicationShortcut
                //再加一个Shortcut只能屏蔽别的Shortcut的onActivated
                //多个Shortcut（以及Action）轮流触发onActivatedAmbiguously
                //并且无论有没有写onActivatedAmbiguously，这个Shortcut都会占一次触发机会
                //sequences: ["+","delete","return","enter"] //"space"
                sequences: ["+"]
                enabled: focus_scope.activeFocus
                onActivated: {
                    console.log('Shortcut 2 onActivated',counter++)
                }
                onActivatedAmbiguously: {
                    console.log('Shortcut 2 onActivatedAmbiguously',counter++)
                }
            }

            //FocusScope中最多一个Item带有focus，
            //如果设置了多个，在第一次切换焦点的时候除了第一个focus，后面的都表现为no focus
            //当FocusScope获得activeFocus时，包含focus的child也将获得activeFocus
            //反之，child activeFocus时，该FocusScope亦获得activeFocus
            FocusScope {
                id: focus_scope
                anchors.fill: parent

                Column {
                    anchors.centerIn: parent
                    spacing: 10
                    Item{
                        id: focus_item
                        width: 100
                        height: 30
                        TextField {
                            anchors.fill: parent
                            focus: true
                            background: Rectangle {
                                border.width: parent.activeFocus?2:1
                                border.color: parent.focus?"red":"gray"
                            }
                        }
                    }
                    Button {
                        width: 100
                        height: 30
                        focus: true
                        background: Rectangle {
                            border.width: parent.activeFocus?2:1
                            border.color: parent.focus?"red":"gray"
                        }
                    }
                    Button {
                        width: 100
                        height: 30
                        focus: true
                        //被Shortcut捕获了就不会触发Keys.press
                        //被Shortcut或者Keys处理了，5.15不会触发click，5.12、5.13会
                        Keys.onSpacePressed: {
                            console.log('Keys onSpacePressed',counter++)
                            event.accepted=true;
                        }
                        onClicked: {
                            console.log('Button onClicked',counter++)
                        }

                        background: Rectangle {
                            border.width: parent.activeFocus?2:1
                            border.color: parent.focus?"red":"gray"
                        }
                    }
                }
            }
        }

        Rectangle {
            id: rect4
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.width: 2
            border.color: keys_scope.activeFocus?"cyan":"white"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log('KeysScope forceActiveFocus.')
                    keys_scope.forceActiveFocus();
                }
            }

            FocusScope {
                id: keys_scope
                anchors.fill: parent
                //enabled为false则不再处理这些输入事件
                //enabled: false
                KeysFilter {
                    id: keys_filter
                    enabled: keys_scope.activeFocus
                    acceptAutoRepeat: false
                    //filterType: KeysFilter.All
                    filterKeys: [Qt.Key_Return,Qt.Key_Enter,Qt.Key_Space,Qt.Key_Plus]
                    onPressed: {
                        console.log('KeysFilter pressed.',key,counter++)
                    }
                }
                //过滤掉ShortcutOverride后，Keys可以正常触发了
                Keys.onPressed: {
                    console.log('Keys out pressed.',event.key,counter++)
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 10
                    Item {
                        width: 100
                        height: 30
                        TextField {
                            anchors.fill: parent
                            focus: true
                            //Keys.forwardTo: keys_scope
                            background: Rectangle {
                                border.width: parent.activeFocus?2:1
                                border.color: parent.focus?"red":"gray"
                            }
                        }
                    }
                    Button {
                        width: 100
                        height: 30
                        focus: true
                        background: Rectangle {
                            border.width: parent.activeFocus?2:1
                            border.color: parent.focus?"red":"gray"
                        }
                    }
                    Button {
                        width: 100
                        height: 30
                        focus: true
                        //被Shortcut捕获了就不会触发Keys.press
                        //被Shortcut或者Keys处理了，5.15不会触发click，5.12、5.13会
                        Keys.onSpacePressed: {
                            console.log('Keys onSpacePressed',counter++)
                            event.accepted=false;
                        }
                        Keys.onPressed: {
                            console.log('Keys in pressed.',event.key,counter++)
                        }

                        onClicked: {
                            console.log('Button onClicked',counter++)
                        }

                        background: Rectangle {
                            border.width: parent.activeFocus?2:1
                            border.color: parent.focus?"red":"gray"
                        }
                    }
                }
            }
        }
    }
}
