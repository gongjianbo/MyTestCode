import QtQuick 2.12
import QtGraphicalEffects 1.0

Rectangle {
    color: "gray"

    Rectangle{
        id: rect
        visible: false
        width: 200
        height: 200
        color: "green"
        Rectangle{
            width: 100
            height: 100
            color: "red"
        }
    }

    OpacityMask{
        anchors.centerIn: parent
        width: 200
        height: 200
        source: rect
        maskSource: Rectangle{
            width: 200
            height: 200
            radius: 20
        }
    }
}
