import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Window {
    width: 600
    height: 400
    x: 210
    y: 210
    visible: true
    title: qsTr("测试QML")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 6
        Item {
            Layout.fillWidth: true
            height: 30
            RowLayout {
                anchors.fill: parent
                spacing: 6
                Label {
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    text: "测试："
                }
                TextField {
                    Layout.fillHeight: true
                    width: 200
                    text: "输入框"
                }
                ComboBox {
                    Layout.fillHeight: true
                    width: 90
                    model: ["下拉1", "下拉2"]
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Button {
                    Layout.fillHeight: true
                    width: 90
                    text: "确定"
                }
            }
        }
        Label {
            Layout.fillWidth: true
            text: "一切都明明白白，但我们仍匆匆错过，因为你相信命运，因为我怀疑生活。"
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Image {
                source: "qrc:/hehua.png"
            }
        }
    }
}
