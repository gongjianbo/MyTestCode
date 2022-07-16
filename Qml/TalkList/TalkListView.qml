import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtMultimedia 5.12
import TalkModel 1.0

//聊天框ListView
//龚建波 2021-3-29
ListView {
    id: control

    //增加部分属性，用于在delegate中访问
    //这样多个view时互不干扰
    property TalkListModel talkModel
    property MediaPlayer audioPlayer

    clip: true
    headerPositioning: ListView.OverlayHeader
    footerPositioning: ListView.OverlayFooter
    boundsBehavior: Flickable.StopAtBounds

    highlightFollowsCurrentItem: true
    highlightMoveDuration: 0
    highlightResizeDuration: 0

    spacing: 10
    delegate: Loader{
        sourceComponent: {
            switch(model.type){
            case TalkData.Text:

                return text_comp;
            case TalkData.Audio:
                return audio_comp;
            }
            return none_comp;
        }

        //放到delegate才能attach model
        Component{
            id: text_comp
            TalkItemText{ }
        }
        Component{
            id: audio_comp
            TalkItemAudio{ }
        }
        Component{
            id: none_comp
            Item{ }
        }
    }

    //相当于头尾边距
    header: Item{
        height: 10
    }
    footer: Item{
        height: 10
    }

    //竖向滚动条
    ScrollBar.vertical: ScrollBar {
        id: scroll_vertical
        contentItem: Item{
            visible: (scroll_vertical.size<1.0)
            implicitWidth: 10
            Rectangle{
                anchors.centerIn: parent
                width: parent.width
                height: parent.height>20?parent.height:20
                color: (scroll_vertical.hovered||scroll_vertical.pressed)
                       ? Qt.darker("#A4ACC6")
                       : "#A4ACC6"
            }
        }
    }
}
