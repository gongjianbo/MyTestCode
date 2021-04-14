import QtQuick 2.12
import QtQuick.Controls 2.12

Button {
    id: control

    property SkinBase skin: SkinNormal

    width: 160
    height: 30

    contentItem: Text {
        text: control.text
        font: control.font
        color: skin.button.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color: skin.button.bgColor
    }
}
