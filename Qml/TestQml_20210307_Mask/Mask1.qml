import QtQuick 2.12
import QtGraphicalEffects 1.0

Rectangle {
    color: "gray"

    Rectangle{
        anchors.centerIn: parent
        width: 200
        height: 200
        color: "green"
        layer.enabled: true
        layer.effect: OpacityMask{
            maskSource: Rectangle{
                width: 200
                height: 200
                radius: 20
            }
        }

        Rectangle{
            width: 100
            height: 100
            color: "red"
        }
    }
}
