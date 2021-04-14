pragma Singleton

import QtQuick 2.12

SkinBase {
    id: control

    rectangle: QtObject{
        property color bgColor: "mintcream"
    }

    button: QtObject{
        property color textColor: "white"
        property color bgColor: "skyblue"
    }
}
