import QtQuick 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12

//自定义提示框
T.Popup {
    id: control

    property color textColor: "black"
    property color currentItemColor: Qt.rgba(0.8,0.8,0.8)
    property color borderColor: "gray"
    property color backgroundColor: Qt.rgba(0.95,0.95,0.95,0.9)
    property int radius: 6
    property alias model: item_list.model

    signal selectText(string text)

    implicitWidth: contentWidth + leftPadding + rightPadding
    //model没数据则不显示
    implicitHeight: (model&&model.length)?contentHeight + topPadding + bottomPadding:0

    font{
        family: "SimSun"
        pixelSize: 14
    }
    padding: 10
    //closePolicy:Popup.CloseOnPressOutsideParent|Popup.CloseOnReleaseOutsideParent|Popup.CloseOnEscape

    contentItem: ListView {
        id: item_list
        //clip: true
        spacing: 2
        implicitHeight: contentHeight
        implicitWidth: 120
        snapMode: ListView.SnapToItem
        delegate: Rectangle{
            implicitHeight: item_text.implicitHeight+12
            implicitWidth: ListView.view.width
            color:ListView.isCurrentItem
                  ?control.currentItemColor
                  :"transparent"
            radius: control.radius
            Text{
                id: item_text
                anchors{
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: 6
                }
                text: modelData||"Error"
                font: control.font
                color: control.textColor
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    item_list.currentIndex=index;
                    control.selectText(item_text.text);
                    control.close();
                }
            }
        }
        onCurrentSectionChanged: {
            console.log(currentSection)
        }
    }

    background: Rectangle {
        color: control.backgroundColor
        border.width: 1
        border.color: control.borderColor
        radius: control.radius
    }

    T.Overlay.modal: Rectangle {
        color: Color.transparent(control.palette.shadow, 0.5)
    }

    T.Overlay.modeless: Rectangle {
        color: Color.transparent(control.palette.shadow, 0.12)
    }
}
