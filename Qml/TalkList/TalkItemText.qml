import QtQuick 2.12

//文本信息delegate
//龚建波 2021-3-29
TalkItemBase {
    id: control

    //ColumnLayout在拉伸时计算有问题，暂时用Column+Row
    Row{
        width: control.contentWidth
        layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
        Rectangle{
            id: wrap_item
            radius: 4
            width: text_item.width
            height: text_item.height
            color: control.messageBgColor

            //指向发言人小三角
            Rectangle{
                width: 10
                height: 10
                y: control.messageHeight/2-10
                anchors.horizontalCenter: control.isUser?parent.right:parent.left
                rotation: 45
                color: control.messageBgColor
            }

            TalkLabel{
                id: text_item
                text: model.text_text
                width: Math.min(control.contentWidth,textWidth)
            }
        }
    }

    Row{
        width: control.contentWidth
        layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
        TalkLabel{
            text: model.datetime
            padding: 0
        }
    }
}
