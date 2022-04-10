import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("GongJianBo Qml Skin")

    EasyRectangle{
        anchors.fill: parent
    }

    Column{
        anchors.centerIn: parent
        spacing: 10
        EasyButton{
            text: "Normal"
            onClicked: {
                SkinManager.currentSkin=SkinNormal;
            }
        }
        EasyButton{
            text: "SkyBlue"
            onClicked: {
                SkinManager.currentSkin=SkinSkyBlue;
            }
        }
        EasyButton{
            text: "Red Button"
            //特殊样式
            skin: SkinManager.currentSkin.redButton
        }
    }
}
