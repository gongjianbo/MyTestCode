import QtQuick 2.12

Rectangle {
    id: control

    property SkinBase skin: SkinNormal
    color: skin.rectangle.bgColor
}
