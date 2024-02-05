import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 650
    height: 470
    visible: true
    title: qsTr("GongJianBo SectionGrid")

    // SimpleGrid { anchors.fill: parent; anchors.margins: 10 }

    SectionGrid { anchors.fill: parent; anchors.margins: 10 }
}
