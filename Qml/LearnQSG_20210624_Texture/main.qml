import QtQuick 2.12
import QtQuick.Window 2.12
import Gt.Component 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Learn QSG")

    //这个背景色设计感觉比较巧妙，所以从twotextureproviders示例拿出来了
    ShaderEffect {
        id: bg
        anchors.fill: parent

        property real tileSize: 16
        property color color1: Qt.rgba(0.9, 0.9, 0.9, 1);
        property color color2: Qt.rgba(0.8, 0.8, 0.8, 1);
        property size pixelSize: Qt.size(width / tileSize, height / tileSize);

        //vec2:pixelSize为画布尺寸和方格大小tileSize的比例，
        //     后面再拿纹理坐标0-1乘上它就相当保持宽高比例的放大，
        //     width/tileSize=7表示显示7列格子，一个格子titleSize宽，
        //     coord*7则原本一个像素现在就是7倍，范围[0-1]变为[0-7]
        //sin(vec2)的值域，vec=0-PI为正，PI-2PI为负，循环
        //     coord*7则sin的值会循环正负正负3.5次
        //sign(x),x>0:y=1,x<0:y=-1,x=0:y=0
        //     这样就把sin的数值变为了1，-1，0便于比较
        //     这样横向循环正负1，竖向循环正负1就形成了一个棋盘
        fragmentShader:
            "
        uniform lowp vec4 color1;
        uniform lowp vec4 color2;
        uniform highp vec2 pixelSize;
        varying highp vec2 qt_TexCoord0;
        void main() {
            highp vec2 tc = sign(sin(3.14159265358979323846 * qt_TexCoord0 * pixelSize));
            if (tc.x != tc.y)
                gl_FragColor = color1;
            else
                gl_FragColor = color2;
        }
        "
    }

    Column{
        x: 10
        y: 10
        spacing: 10
        Row{
            spacing: 10
            //点击文字切换item或者更新颜色
            Rectangle{
                width: 100
                height: 50
                color: "transparent"
                border.color: "red"
                Text{
                    id: txt
                    font.pixelSize: 40
                    text: "Hello"
                    layer.enabled: true
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        txt.color = Qt.hsla(Math.random(),0.5,0.5,1);
                        texture.source = txt;
                    }
                }
            }

            Rectangle{
                width: 100
                height: 50
                color: "transparent"
                border.color: "red"
                Text{
                    id: txt2
                    font.pixelSize: 40
                    text: "QSG"
                    layer.enabled: true
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        txt2.color = Qt.hsla(Math.random(),0.5,0.5,1);
                        texture.source = txt2;
                    }
                }
            }

            Rectangle{
                width: 100
                height: 50
                color: "transparent"
                border.color: "red"
                MyTextureItem{
                    id: texture
                    width: source.width
                    height: source.height
                    source: txt
                }
            }
        }//end Row

        Row{
            spacing: 10
            Rectangle{
                width: 100
                height: 50
                color: "transparent"
                border.color: "red"
                Item {
                    id: box
                    width: 100
                    height: 50

                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width * 0.9
                        height: parent.width * 0.4
                        radius: width * 0.1;
                        gradient: Gradient {
                            GradientStop { position: 0; color: Qt.hsla(0.6, 0.9, 0.9, 1); }
                            GradientStop { position: 1; color: Qt.hsla(0.6, 0.6, 0.3, 1); }
                        }
                        //RotationAnimator on rotation { from: 0; to: 360; duration: 10000; loops: Animation.Infinite }
                    }
                    //visible: false
                    layer.enabled: true
                }
            }

            Rectangle{
                width: 100
                height: 50
                color: "transparent"
                border.color: "red"
                Item {
                    id: text
                    width: box.width
                    height: box.height
                    Text {
                        anchors.centerIn: parent
                        color: "black" // Qt.hsla(0.8, 0.8, 0.8);
                        text: "Qt\nQuick"

                        horizontalAlignment: Text.AlignHCenter

                        font.bold: true
                        font.pixelSize: text.width * 0.25
                        //RotationAnimator on rotation { from: 360; to: 0; duration: 9000; loops: Animation.Infinite }
                    }
                    //visible: false
                    layer.enabled: true
                }
            }

            Rectangle{
                width: 100
                height: 50
                color: "transparent"
                border.color: "red"
                MyBlenderItem{
                    id: blender
                    width: box.width
                    height: box.height
                    source1: box
                    source2: text
                }
            }
        }
    }//end Column
}
