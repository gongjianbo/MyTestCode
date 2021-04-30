import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Shader Effect")

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
gl_FragColor = vec4(od.g,od.g,od.g,od.a);
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

        MyShaderEffect{
            width: img.width
            height: img.height
            property var source: img
            property real radius: 80
            property real rotate: 0
            property real centerX: width/2+20*Math.cos(rotate*(Math.PI/180))
            property real centerY: height/2+20*Math.sin(rotate*(Math.PI/180))

            NumberAnimation on rotate {
                loops: -1
                from: 0
                to: 360
                duration: 3000
            }

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
uniform float radius;
uniform float centerX;
uniform float centerY;
vec2 tc=qt_TexCoord0;
vec4 od=vec4(0,0,0,1);
                    void main() {
float val=1-abs(sqrt(pow(centerX-vt.x,2.0)+pow(centerY-vt.y,2.0)))/radius;
if(val<0.4)
val=0.3;
od.rgb=vec3(val,val,val);

gl_FragColor = texture2D(source, qt_TexCoord0) * od * qt_Opacity;
                    }"
        }
    }
}
