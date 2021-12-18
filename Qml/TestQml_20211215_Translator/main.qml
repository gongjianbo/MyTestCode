import QtQuick 2.12
import QtQml 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as Ctrl1
import QtQuick.Layouts 1.12
import Trans 1.0

Window {
    width: 820
    height: 640
    visible: true
    title: qsTr("Qt Translator")

    Column {
        x: 20
        y: 10
        spacing: 10
        //第一行，切换按钮
        Row {
            spacing: 20
            RadioButton {
                text: "中文"
                checked: translator.language === Translator.ZH_CN
                onClicked: {
                    translator.language = Translator.ZH_CN
                }
            }
            RadioButton {
                text: "English"
                checked: translator.language === Translator.EN_US
                onClicked: {
                    translator.language = Translator.EN_US
                }
            }
        }
        //第二行：文本
        Row {
            spacing: 20
            Text {
                text: qsTr("翻译")
            }
            Text {
                text: "翻译 " + qsTr("翻译")
            }
            Text {
                text: qsTr("翻译 %1").arg(qsTr("翻译"))
            }
        }
        //第三行：Control组件
        Row {
            spacing: 20
            Button {
                text: qsTr("翻译")
            }
            TextField {
                placeholderText: qsTr("翻译")
            }
            ComboBox {
                currentIndex: 2
                model: [qsTr("第一项"), qsTr("第二项"), qsTr("第三项")]
            }
            ComboBox {
                Component.onCompleted: {
                    model = [qsTr("第一项"), qsTr("第二项"), qsTr("第三项")]
                    currentIndex = 2
                }
            }
        }
        //第四行：Control组件
        Row {
            spacing: 20
            ComboBox {
                currentIndex: 2
                model: comboModel
            }
            ComboBox {
                currentIndex: 2
                model: listModel
                textRole: "txt"
            }
            ComboBox {
                currentIndex: 2
                model: listModel
                textRole: "trans"
            }
            ComboBox {
                currentIndex: 2
                model: ListModel {
                    ListElement {
                        text: qsTr("第一项")
                    }
                    ListElement {
                        text: qsTr("第二项")
                    }
                    ListElement {
                        text: qsTr("第三项")
                    }
                }
            }
            ComboBox {
                model: qml_combomodel
                ListModel {
                    id: qml_combomodel
                }
                Component.onCompleted: {
                    qml_combomodel.append({"text":qsTr("第一项")})
                    qml_combomodel.append({"text":qsTr("第二项")})
                    qml_combomodel.append({"text":qsTr("第三项")})
                    currentIndex = 2
                }
            }
        }
        //第五行，列表
        ListView {
            width: 300
            height: contentHeight
            model: listModel
            spacing: 1
            delegate: Rectangle {
                height: 40
                width: ListView.view.width
                border.color: "black"
                Row {
                    x: 20
                    spacing: 20
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        text: qsTr("翻译")
                    }
                    Text {
                        text: model.txt
                    }
                    Text {
                        text: model.trans
                    }
                }
            }
        }
        //第六行
        TabBar {
            id: tab_bar
            width: 300
            TabButton {
                text: qsTr("第一项")
            }
            TabButton {
                text: qsTr("第二项")
            }
            TabButton {
                text: qsTr("第三项")
            }
        }
        ListView {
            height: 40
            width: contentWidth
            orientation: ListView.Horizontal
            model: stack_layout.children
            spacing: 1
            //不能绑定currentIndex，不然翻译后modelChanged就成了默认值
            //currentIndex: stack_layout.currentIndex
            delegate: Rectangle {
                width: 90
                height: ListView.view.height
                //color: ListView.isCurrentItem ? "gray" : "white"
                color: stack_layout.currentIndex === model.index ? "gray" : "white"
                border.color: "black"
                Text {
                    anchors.centerIn: parent
                    text: model.title
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: tab_bar.currentIndex = model.index
                }
            }
        }
        StackLayout {
            id: stack_layout
            width: 300
            height: 200
            currentIndex: tab_bar.currentIndex
            Rectangle {
                border.color: "black"
                property string title: qsTr("第一项")
                Text {
                    text: qsTr("第一项")
                }
            }
            Rectangle {
                border.color: "black"
                property string title: qsTr("第二项")
                Text {
                    text: qsTr("第二项")
                }
            }
            Rectangle {
                border.color: "black"
                property string title: qsTr("第三项")
                Text {
                    text: qsTr("第三项")
                }
            }
        }
    }

    Ctrl1.Calendar {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        //默认值为Qt.locale()不能动态切换
        locale: translator.locale
    }
}
