import QtQuick 2.12

//参照TaoQuick
ShaderEffect {
    id: control

    // 如果环境是OpenGL ES2，默认的version是 version 110, 不需要写出来。
    // 比ES2更老的版本是ES 1.0 和 ES 1.1, 忽略。
    // ES2没有texture函数，要用旧的texture2D代替
    // #version指令必须在着色器顶部，在它之前只能有注释和空白
    readonly property string gles2Ver: "
#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif
#define texture texture2D
"

    // 如果环境是OpenGL ES3，version是 version 300 es
    // ES 3.1 ES 3.2也可以。
    // ES3可以用in out 关键字，gl_FragColor用out fragColor取代
    readonly property string gles3Ver: "#version 300 es
#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif
#define varying in
#define gl_FragColor fragColor

out vec4 fragColor;
"

    // 如果环境是OpenGL Desktop 2.x，version这里就用2.0的version 110，即2.0版本
    // 2.x 没有texture函数，要用旧的texture2D代替
    readonly property string gl2Ver: "#version 110
#define texture texture2D
"

    // 如果环境是OpenGL Desktop 3.x，version这里参考Qt默认的version 150。大部分Desktop设备应该都是150
    // 150 即3.2版本，第一个区分Core和Compatibility的版本。
    // Core是核心模式，只有核心api以减轻负担。相应的Compatibility是兼容模式，保留全部API以兼容低版本。
    // 可以用in out 关键字，gl_FragColor用out fragColor取代
    // 精度限定抹掉，用默认的。不抹掉有些情况下会报错，不能通用。
    readonly property string gl3Ver: "#version 150
#define varying in
#define gl_FragColor fragColor
#define lowp
#define mediump
#define highp

out vec4 fragColor;
"


    property string head: {
        console.log("versionString",
                    GraphicsInfo.majorVersion,
                    GraphicsInfo.minorVersion,
                    GraphicsInfo.renderableType)
        //if (Qt.platform.os === "android")
        if(GraphicsInfo.renderableType===GraphicsInfo.SurfaceFormatOpenGLES) {
            if (GraphicsInfo.majorVersion < 3) {
                return gles2Ver
            } else {
                return gles3Ver
            }
        } else {
            if (GraphicsInfo.majorVersion < 3) {
                return gl2Ver;
            } else {
                return gl3Ver;
            }
        }
    }

    // default vertex shader code
    vertexShader: "#version 150
            uniform mat4 qt_Matrix;
            in vec4 qt_Vertex;
            in vec2 qt_MultiTexCoord0;
            out vec2 qt_TexCoord0;
            void main() {
                qt_TexCoord0 = qt_MultiTexCoord0;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
    // default fragment shader code
    fragmentShader: "#version 150
            uniform sampler2D source;
            uniform float qt_Opacity;
            in vec2 qt_TexCoord0;
            out vec4 fragColor;
            void main() {
                fragColor = texture(source, qt_TexCoord0) * qt_Opacity;
            }"
}
