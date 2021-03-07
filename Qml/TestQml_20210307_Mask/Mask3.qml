import QtQuick 2.12
import QtGraphicalEffects 1.0

Rectangle {
    color: "gray"

    Rectangle{
        id: rect
        visible: false
        radius: 20
        width: 200
        height: 200
        color: "green"

        Rectangle{
            width: 100
            height: 100
            color: "red"
        }
    }

    ShaderEffect{
        anchors.centerIn: parent
        width: 200
        height: 200
        property var source: ShaderEffectSource{
            sourceItem: rect
        }
        property var mask: ShaderEffectSource{
            sourceItem: Rectangle{
                width: 200
                height: 200
                radius: 20
            }
        }

        vertexShader: "
            uniform highp mat4 qt_Matrix;
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            varying highp vec2 coord;
            void main() {
                coord = qt_MultiTexCoord0;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: "
            varying highp vec2 coord;
            uniform lowp float qt_Opacity;
            uniform sampler2D source;
            uniform sampler2D mask;
            void main() {
                gl_FragColor = texture2D(source, coord) * texture2D(mask, coord) * qt_Opacity;
            }"
    }
}
