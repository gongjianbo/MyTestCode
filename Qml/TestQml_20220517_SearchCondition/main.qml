import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Test 1.0

Window {
    width: 1200
    height: 800
    visible: true
    title: qsTr("Search Condition")

    Rectangle {
        anchors.fill: condition_view
        anchors.margins: -1
        border.color: "black"
        radius: 4
        color: "transparent"
    }

    SearchConditionModel {
        id: condition_model
    }

    ListView {
        id: condition_view
        anchors.fill: parent
        anchors.margins: 24
        spacing: 1
        model: condition_model
        clip: true

        delegate: Rectangle {
            width: ListView.view.width
            height: 50
            color: "gray"
            radius: 4
            Row {
                y: 5
                spacing: 6
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white"
                    text: "分组"+(model.index+1)
                }
                //条件1
                Row {
                    spacing: 6
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: "white"
                        text: "条件1"
                    }
                    ComboBox {
                        model: ["满足任意一个","同时满足"]
                        currentIndex: c1MatchAll?1:0
                        onCurrentIndexChanged: {
                            c1MatchAll=(currentIndex===0?false:true);
                            //console.log('match change',c1MatchAll)
                        }
                    }
                    TextField {
                        text: model.c1Keywords
                        onTextChanged: {
                            model.c1Keywords=text;
                            console.log('keyword change',model.c1Keywords)
                        }
                    }
                }

                //条件2
                Row {
                    visible: model.enableC2
                    spacing: 6
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: "white"
                        text: "条件2"
                    }
                    ComboBox {
                        model: ["满足任意一个","同时满足"]
                        currentIndex: c2MatchAll?1:0
                        onCurrentIndexChanged: {
                            c2MatchAll=currentIndex===0?false:true;
                        }
                    }
                    TextField {
                        text: model.c2Keywords
                        onTextChanged: {
                            model.c2Keywords=text;
                        }
                    }
                    Button {
                        text: "删除条件"
                        onClicked: {
                            condition_model.disableCondition2(model.index);
                        }
                    }
                    ComboBox {
                        model: ["满足任意一个条件","条件同时满足"]
                        currentIndex: matchAll?1:0
                        onCurrentIndexChanged: {
                            matchAll=currentIndex===0?false:true;
                        }
                    }
                }
                Button {
                    visible: !model.enableC2
                    text: "新建条件"
                    onClicked: {
                        condition_model.enableCondition2(model.index);
                    }
                }
                Button {
                    visible: condition_view.count>1
                    text: "删除分组"
                    onClicked: {
                        condition_model.removeGroup(model.index);
                    }
                }
            }
        }

        footerPositioning: ListView.OverlayFooter
        footer: Rectangle {
            width: ListView.view.width
            height: 50
            z: 2
            color: Qt.darker("gray")
            radius: 4
            Row {
                anchors.centerIn: parent
                spacing: 12
                Button {
                    text: "新建分组"
                    onClicked: {
                        condition_model.appendGroup();
                    }
                }
                Button {
                    text: "开始处理"
                    onClicked: {
                        condition_model.checkAndProcess();
                    }
                }
                Item {
                    width: 20
                    height: 20
                }
                Button {
                    text: "保存记录"
                    onClicked: {
                        condition_model.saveCondition();
                    }
                }
                Button {
                    text: "重置"
                    onClicked: {
                        condition_model.reset();
                    }
                }
                Button {
                    text: "恢复记录"
                    onClicked: {
                        condition_model.restoreCondition();
                    }
                }
            }
        }
    }
}
