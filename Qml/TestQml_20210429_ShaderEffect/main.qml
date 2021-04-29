import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Grid{
        spacing: 10
        anchors.centerIn: parent
        columns: 3
        rows: 3

        Image{
            id: img
            source: "qrc:/hua.png"
        }

        ShaderEffect{
            width: img.width
            height: img.height
            property var source: img
        }

        GaussianBlur{
            width: img.width
            height: img.height
            source: img
        }

        MyShaderEffect{
            width: img.width
            height: img.height
            property var source: img
            fragment: "
vec4 od;
                    void main() {
od = texture2D(source, qt_TexCoord0) * qt_Opacity;
gl_FragColor = vec4(od.r,od.r,od.r,od.a);
                    }"
        }

        MyShaderEffect{
            width: img.width
            height: img.height
            property var source: img
            fragment: "
vec4 od;
                    void main() {
od = texture2D(source, qt_TexCoord0) * qt_Opacity;
gl_FragColor = vec4(od.g,od.g,od.g,od.a);
                    }"
        }

        MyShaderEffect{
            width: img.width
            height: img.height
            property var source: img
            fragment: "
vec4 od;
                    void main() {
od = texture2D(source, qt_TexCoord0) * qt_Opacity;
gl_FragColor = vec4(od.b,od.b,od.b,od.a);
                    }"
        }

        MyShaderEffect{
            width: img.width
            height: img.height
            property var source: img
            fragment: "
                    void main() {
gl_FragColor = texture2D(source, qt_TexCoord0) * vec4(1,0,0,1) * qt_Opacity;
                    }"
        }

        MyShaderEffect{
            width: img.width
            height: img.height
            property var source: img

            vertex: "
out vec4 vt;
                            void main() {
vt=qt_Vertex;
                                qt_TexCoord0 = qt_MultiTexCoord0;
                                gl_Position = qt_Matrix * qt_Vertex;
                            }"

            //qt_Vertex (pos.x,pos.y,0,1)
            fragment: "
in vec4 vt;
vec4 od=vec4(0,0,0,1);
vec2 tc=qt_TexCoord0;
                    void main() {

if(int(vt.x)%20>=10)
od.rg=vec2(1,1);
else
od.gb=vec2(1,1);

gl_FragColor = texture2D(source, qt_TexCoord0) * od * qt_Opacity;
                    }"
        }
    }
}
