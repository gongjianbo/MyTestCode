import QtQuick 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12

//自定义按钮
T.Button {
    id:control

    property color textColor: enabled?"white":"gray"
    property color backgroundTheme: Qt.rgba(22/255,155/255,213/255,1)
    property color backgroundColor: control.down
                                    ? Qt.darker(backgroundTheme)
                                    : (control.hovered||control.highlighted)
                                      ? Qt.lighter(backgroundTheme)
                                      : control.checked
                                        ? backgroundTheme
                                        : backgroundTheme


    font{
        family: "SimHei"
        pixelSize: 16
    }

    contentItem: Text{
            anchors{
                verticalCenter: parent.verticalCenter
            }
            text: control.text
            color: control.textColor
            //renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font: control.font
        }

    background: Rectangle{
        implicitWidth: control.implicitWidth
        implicitHeight: control.implicitHeight
        radius: 5
        color: control.backgroundColor
    }
}
