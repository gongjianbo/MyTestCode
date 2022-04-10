import QtQuick 2.12
import QtQml 2.12

QtObject {
    property color textColor: "white"
    property color bgNormalColor: "gray"
    property color bgHoverColor: Qt.lighter(bgNormalColor, 1.2)
    property color bgPressedColor: Qt.darker(bgNormalColor, 1.8)
}
