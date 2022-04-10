import QtQuick 2.12
import QtQml 2.12
import "./SkinElement"

Rectangle {
    id: control

    property RectangleStyle skin: SkinManager.currentSkin.rectangle

    color: skin.bgColor
}
