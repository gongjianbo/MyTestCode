import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 520
    height: 480
    visible: true
    title: qsTr("Hello Qt6 Calendar")

    MyCalendar {
        anchors.centerIn: parent
    }
}
