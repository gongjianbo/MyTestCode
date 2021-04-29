import QtQuick 2.12


ShaderEffect {
    id: control

    property var source: Image{ }

    property string vertex: "
                    void main() {
                        qt_TexCoord0 = qt_MultiTexCoord0;
                        gl_Position = qt_Matrix * qt_Vertex;
                    }"

    property string fragment: "
                    void main() {
#ifdef GL_ES
gl_FragColor = texture2D(source, qt_TexCoord0)*vec4(1,0,0,1) * qt_Opacity;
#else
gl_FragColor = texture2D(source, qt_TexCoord0)* qt_Opacity;
#endif

                    }"


    //default vertex shader code
    vertexShader: "#version 150
#define varying out
#define attribute in

                    uniform mat4 qt_Matrix;
                    in vec4 qt_Vertex;
                    in vec2 qt_MultiTexCoord0;
                    out vec2 qt_TexCoord0;
                    "+vertex


    //default fragment shader code
    fragmentShader: "#version 150
#define varying in
#define texture2D texture
out vec4 fragColor;
#define gl_FragColor fragColor

                    in vec2 qt_TexCoord0;
                    uniform sampler2D source;
                    uniform float qt_Opacity;
                    "+fragment

    property string versionString: {
        console.log("version string",GraphicsInfo.majorVersion,GraphicsInfo.minorVersion)
        if (Qt.platform.os === "android") {
            if (GraphicsInfo.majorVersion === 3) {
                console.log("android gles 3")
                return "es3"
            } else {
                console.log("android gles 2")
                return "es2"
            }
        } else {
            if (GraphicsInfo.majorVersion === 3 ||GraphicsInfo.majorVersion === 4) {
                return "gl3"
            } else {
                return "gl2"
            }
        }
    }

    Component.onCompleted: {
        console.log("completed version",GraphicsInfo.majorVersion,GraphicsInfo.minorVersion);
    }
}
