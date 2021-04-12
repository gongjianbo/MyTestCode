import QtQuick 2.12
import QtMultimedia 5.12

import TalkModel 1.0

//音频信息delegate
//龚建波 2021-3-29
TalkItemBase {
    id: control

    //ColumnLayout在拉伸时计算有问题，暂时用Column+Row
    Row{
        width: control.contentWidth
        layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
        height: 50
        spacing: 12

        Rectangle{
            radius: 4
            width: 80+Math.min(210,Math.pow(model.audio_duration/1000,0.5)*20)
            height: control.messageHeight
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

            Row{
                anchors{
                    verticalCenter: parent.verticalCenter
                    margins: 14
                    //left和right这样互斥写，属性绑定会有问题
                    //但是layoutDirection切换后会刷新
                    left: control.isUser?undefined:parent.left
                    right: control.isUser?parent.right:parent.undefined
                }
                layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
                spacing: 0
                AnimatedImage{
                    anchors.verticalCenter: parent.verticalCenter
                    source: (audioPlayer.playbackState===MediaPlayer.PlayingState&&
                             audioPlayer.currentId===index)
                            ?"qrc:/Image/audio_playing.gif"
                            :"qrc:/Image/audio_20_play.png"
                    rotation: control.isUser?180:0
                    //切换source后不会自动播放
                    onSourceChanged: playing=true
                }
                TalkLabel{
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.audio_durationStr
                }
            }

            //点击播放
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(audioPlayer.playbackState===MediaPlayer.PlayingState){
                        audioPlayer.stop();
                    }else{
                        //source没变不能再次播放？
                        audioPlayer.source="";
                        audioPlayer.source=model.audio_path;
                        audioPlayer.currentId=index;
                        audioPlayer.play();
                    }
                }
            }
        }

        //转换按钮
        Rectangle{
            visible: (model.status!==TalkData.ParseSuccess)
            anchors.verticalCenter: parent.verticalCenter
            //之前放在消息rect内部了，现在作为兄弟
            //anchors{
            //    verticalCenter: parent.verticalCenter
            //    margins: control.isUser?-width-12:12
            //    left: control.isUser?parent.left:parent.right
            //    //right: control.isUser?parent.left:parent.undefined
            //}
            height: control.messageHeight-16
            width: label_item.width+24
            radius: height/2
            color: "#A4ACC6"

            TalkLabel{
                id: label_item
                anchors.centerIn: parent
                color: "white"
                text: {
                    switch(model.status)
                    {
                    case TalkData.DataError: return "无效数据";
                    case TalkData.DataReady: return "转文字";
                    case TalkData.ParseOn: return "      ";
                    case TalkData.ParseSuccess: return "转换成功";
                    case TalkData.ParseError: return "转换失败";
                    }
                    return "      ";
                }
            }

            AnimatedImage{
                anchors.centerIn: parent
                visible: (model.status===TalkData.ParseOn)
                source: "qrc:/Image/audio_loading.gif"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    //直接通过id访问外部对象
                    //需要处理当前是否可以点击
                    talkModel.parseRow(index);
                }
            }
        }//end 转换按钮
    }

    Row{
        width: control.contentWidth
        layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
        TalkLabel{
            text: model.datetime
            padding: 0
        }
    }

    Row{
        visible: (model.status===TalkData.ParseSuccess)
        width: control.contentWidth
        layoutDirection: control.isUser?Qt.RightToLeft:Qt.LeftToRight
        Rectangle{
            id: wrap_item
            radius: 4
            width: text_item.width
            height: text_item.height
            color: control.messageBgColor

            //无文本时的警告图片
            Image {
                visible: model.audio_text.length<=0
                x: 14
                //有些情况下图片显示异常，设置透明度0.9解决了
                opacity: 0.999
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/Image/warn_20_red.png"
            }

            TalkLabel{
                id: text_item
                text: model.audio_text.length>0?model.audio_text:"       未识别到文字"
                width: Math.min(control.contentWidth,textWidth)
            }
        }
    }
}
