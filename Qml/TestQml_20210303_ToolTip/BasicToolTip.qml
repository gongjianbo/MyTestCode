import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Templates 2.12 as T

T.ToolTip {
    id: control

    x: parent ? Number.parseInt((parent.width - implicitWidth) / 2) : 0
    y: -implicitHeight - 2

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    margins: 6
    verticalPadding: 8
    horizontalPadding: 12

    closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutsideParent | T.Popup.CloseOnReleaseOutsideParent

    font{
        family: "Microsoft YaHei"
        pixelSize: 14
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: "red"
    }

    background: Rectangle {
        border.color: "red"
        color: "white"
        radius: 4
    }
}
