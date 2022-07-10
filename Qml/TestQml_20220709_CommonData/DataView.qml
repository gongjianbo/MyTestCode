import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 2.12
import Test 1.0

Rectangle {
    id: control

    ListView {
        anchors.fill: parent
        anchors.margins: 10
        anchors.bottomMargin: 50
        clip: true
        spacing: 2
        model: data_model
        delegate: data_component
    }

    Component {
        id: data_component
        Rectangle {
            width: ListView.view.width
            height: 40
            border.color: "black"
            color: "gray"

            Row {
                x: 10
                anchors.verticalCenter: parent.verticalCenter
                spacing: 6
                TextField {
                    id: text_key
                    width: 90
                    height: 30
                    text: model.key
                }
                SpinBox {
                    id: spin_val
                    width: 120
                    height: 30
                    from: 0
                    to: 1000000
                    value: model.value
                }
                Button {
                    width: 60
                    height: 30
                    text: "update"
                    onClicked: {
                        //update更新所有view中的该item
                        data_model.updateItemByRow(model.index, text_key.text, spin_val.value)
                    }
                }
                Button {
                    width: 60
                    height: 30
                    text: "delete"
                    onClicked: {
                        //删除item
                        data_model.deleteItemByRow(model.index)
                    }
                }
                Button {
                    width: 60
                    height: 30
                    text: "clone"
                    onClicked: {
                        //以当前值新增一个item
                        data_model.appendItem(model.key, model.value)
                    }
                }
            }
        }

    }

    Row {
        anchors.bottom: parent.bottom
        spacing: 6
        SpinBox {
            id: value_min
            from: 0
            to: 1000000
            value: 0

        }
        SpinBox {
            id: value_max
            from: 0
            to: 1000000
            value: 10000
        }
        Button {
            text: "filter"
            onClicked: {
                //根据value的范围过滤
                //数据中有10001，而默认范围10000，测试时加1并重新过滤
                data_model.searchItemByValue(value_min.value, value_max.value);
            }
        }
    }

    DataModel {
        id: data_model
        Component.onCompleted: {
            //初始化时过滤一次
            data_model.searchItemByValue(value_min.value, value_max.value);
        }
    }
}
