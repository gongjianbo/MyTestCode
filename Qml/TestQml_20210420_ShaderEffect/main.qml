import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0

Window {
    id: root
    width: 900
    height: 800
    visible: true
    title: qsTr("Learn ShaderEffect")

    Grid {
        anchors.centerIn: parent
        spacing: 20
        rows: 3
        columns: 3
        Image {
            id: img
            width: 200
            height: width
            source: "qrc:/hua.png"
        }
        ShaderEffect {
            id: effect
            width: img.width
            height: img.height
            property variant source: img
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log(parent.vertexShader)
                    console.log(parent.fragmentShader)
                }
            }
        }
        /* ShaderEffect {
            id: effect2
            width: img.width
            height: img.height
            // the source where the effect shall be applied to
            property variant source: img
            // default vertex shader code
            vertexShader: "
                    uniform highp mat4 qt_Matrix;
                    attribute highp vec4 qt_Vertex;
                    attribute highp vec2 qt_MultiTexCoord0;
                    varying highp vec2 qt_TexCoord0;
                    void main() {
                        qt_TexCoord0 = qt_MultiTexCoord0;
                        gl_Position = qt_Matrix * qt_Vertex;
                    }"
            // default fragment shader code
            //texture2D 第一个参数是采样器，
            fragmentShader: "
                    varying highp vec2 qt_TexCoord0;
                    uniform sampler2D source;
                    uniform lowp float qt_Opacity;
                    void main() {
                        gl_FragColor = texture2D(source, qt_TexCoord0) * qt_Opacity;
                    }"
        }
        ShaderEffect {
            id: effect3
            width: img.width
            height: img.height
            property variant source: img
            fragmentShader: "
                        varying highp vec2 qt_TexCoord0;
                        uniform sampler2D source;
                        uniform lowp float qt_Opacity;
                        void main() {
                            gl_FragColor = texture2D(source, qt_TexCoord0) * vec4(1.0, 0.0, 0.0, 1.0) * qt_Opacity;
                        }"
        }
        ShaderEffect {
            id: effect4
            width: img.width
            height: img.height
            property variant source: img
            fragmentShader: "
                       varying highp vec2 qt_TexCoord0;
                       uniform sampler2D source;
                       uniform lowp float qt_Opacity;
                       void main() {
                           gl_FragColor = texture2D(source, qt_TexCoord0) * vec4(qt_TexCoord0.x, qt_TexCoord0.x, qt_TexCoord0.x, 1.0) * qt_Opacity;
                       }"
        }
        ShaderEffect {
            id: effect42
            width: img.width
            height: img.height
            property variant source: img
            fragmentShader: "
                       varying highp vec2 qt_TexCoord0;
                       uniform sampler2D source;
                       uniform lowp float qt_Opacity;
                       void main() {
                           gl_FragColor = texture2D(source, qt_TexCoord0) * vec4(qt_TexCoord0.y, qt_TexCoord0.y, qt_TexCoord0.y, 1.0) * qt_Opacity;
                       }"
        }
        ShaderEffect {
            id: effect5
            width: img.width
            height: img.height
            property variant source: img
            property real frequency: 8
            property real amplitude: 0.1
            property real time: 0.0
            NumberAnimation on time {
                from: 0; to: Math.PI*2; duration: 1000; loops: Animation.Infinite
            }

            fragmentShader: "
                        varying highp vec2 qt_TexCoord0;
                        uniform sampler2D source;
                        uniform lowp float qt_Opacity;
                        uniform highp float frequency;
                        uniform highp float amplitude;
                        uniform highp float time;
                        void main() {
                            highp vec2 pulse = sin(time - frequency * qt_TexCoord0);
                            highp vec2 coord = qt_TexCoord0 + amplitude * vec2(pulse.x, -pulse.x);
                            gl_FragColor = texture2D(source, vec2(-0.5f+coord.x*2.0f,-0.5f+coord.y*2.0f)) * qt_Opacity;
                        }"
        }*/

        MyShader{
            id: effect6
            width: img.width
            height: img.height
            property variant source: img
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log(parent.vertexShader)
                    console.log(parent.fragmentShader)
                }
            }
        }

        DirectionalBlur {
            width: img.width
            height: img.height
            source: img
            angle: 90
            length: 32
            samples: 24
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log(parent.vertexShader)
                    console.log(parent.fragmentShader)
                }
            }
        }
    }
}
