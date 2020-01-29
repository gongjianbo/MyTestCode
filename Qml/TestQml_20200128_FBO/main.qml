import QtQuick 2.12
import QtQuick.Window 2.12

import LearnOpenGL 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Learn OpenGL")
    color: "darkCyan"

    FBOItem{
        width: 100
        height: 100
    }

    FBOItem{
        anchors.fill: parent
        anchors.margins: 50
    }
}
