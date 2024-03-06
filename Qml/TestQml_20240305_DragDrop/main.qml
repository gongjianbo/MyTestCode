import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("GongJianBo")

    // TestInternal { anchors.fill: parent }
     TestAutomatic { anchors.fill: parent }
    // TestDragHandler { anchors.fill: parent }
    // TestNone { anchors.fill: parent }
}
