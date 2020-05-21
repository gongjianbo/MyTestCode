import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("龚建波 1992")

   header: TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Hollow")
        }
        TabButton {
            text: qsTr("Solid")
        }
        TabButton {
            text: qsTr("Circle")
        }
    }

    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex
        anchors.fill: parent

        HollowHeart{
        }
        SolidHeart{
        }
        CircleHeart{
        }
    }
}
