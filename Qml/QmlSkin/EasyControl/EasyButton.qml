import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 2.12
import "./SkinElement"

Button {
    id: control

    property ButtonStyle skin: SkinManager.currentSkin.button

    width: 160
    height: 30

    contentItem: Text {
        text: control.text
        font: control.font
        color: skin.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color: pressed
               ? skin.bgPressedColor
               : hovered
                 ? skin.bgHoverColor
                 : skin.bgNormalColor
    }
}
