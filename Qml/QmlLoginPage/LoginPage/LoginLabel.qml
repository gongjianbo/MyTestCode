import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Templates 2.12 as T

//自定义可点击的label
T.AbstractButton {
    id: control

    property color textTheme: Qt.rgba(22/255,155/255,213/255,1)
    property color textColor: control.down
           ? Qt.darker(textTheme)
           : (control.hovered||control.highlighted)
             ? Qt.lighter(textTheme)
             : control.checked
               ? textTheme
               : textTheme

    implicitHeight: btn_text.implicitHeight
    implicitWidth: btn_text.implicitWidth
    font{
        family: "SimHei"
        pixelSize: 13
    }

    //renderType: Text.NativeRendering
    Text {
        id: btn_text
        text: control.text
        color: control.textColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font: control.font
    }
}
