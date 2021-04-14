import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("GongJianBo Qml Skin")

    property SkinBase current_skin: SkinNormal

    EasyRectangle{
        anchors.fill: parent
        skin: current_skin
    }

    Column{
        anchors.centerIn: parent
        spacing: 10
        EasyButton{
            text: "Normal"
            skin: current_skin
            onClicked: {
                current_skin=SkinNormal;
            }
        }
        EasyButton{
            text: "SkyBlue"
            skin: current_skin
            onClicked: {
                current_skin=SkinSkyBlue;
            }
        }
    }
}
