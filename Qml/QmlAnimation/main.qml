import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    menuBar: MenuBar{
        Menu{
            title: "Demo"
            Action{
                text: "animation"
                onTriggered: root_loader.setSource("qrc:/AnimationDemo.qml")
            }
            Action{
                text: "state"
                onTriggered: root_loader.setSource("qrc:/StateDemo.qml")
            }
            Action{
                text: "transition"
                onTriggered: root_loader.setSource("qrc:/TransitionDemo.qml")
            }
        }
    }

    Loader{
        id: root_loader
        anchors.fill: parent
        anchors.margins: 10
        source: "qrc:/TransitionDemo.qml"
    }
}
