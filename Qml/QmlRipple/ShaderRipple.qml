import QtQuick 2.12

//使用shader制作波纹效果
ShaderEffect{
    id: control
    width: 150
    height: 150
    //动画移动偏移量
    property real offset: 0
    //波纹间隔
    property real spacing: 20

    NumberAnimation {
        target: control
        property: "offset"
        duration: 2000
        from: 0
        to: control.spacing
        loops: Animation.Infinite //-1
        running: true
    }

    vertexShader: "
uniform mat4 qt_Matrix;
attribute vec4 qt_Vertex;
attribute vec2 qt_MultiTexCoord0;
varying vec2 thePos;

void main() {
thePos =  vec2(qt_MultiTexCoord0.x*2.0-1.0,-qt_MultiTexCoord0.y*2.0+1.0);
gl_Position = qt_Matrix * qt_Vertex;
}
"

    fragmentShader: "
varying vec2 thePos;
uniform float qt_Opacity;
uniform float width;
uniform float offset;
uniform float spacing;
vec4 outColor;
float mod(float x,float y)
{
return x-y*floor(x/y);
}
void main()
{
outColor = vec4(1,0,0,0);
float aSmoothWidth=20.0/float(width);
//distance(pos)=[0,1],on center=0,on border=1
float dis=distance(thePos,vec2(0.0,0.0))*width;
float maxlen=float(int(width)/int(spacing*2.0))*spacing*2.0-spacing*2.0+offset*2.0;
if(dis<=maxlen){
outColor.a=mod(dis-offset*2.0,spacing*2.0)/(spacing*2.0);
if(outColor.a>0.99-aSmoothWidth)
outColor.a=smoothstep(0.99,0.98-aSmoothWidth,outColor.a);
outColor.a*=outColor.a;
}
gl_FragColor = outColor * qt_Opacity;
gl_FragColor.rgb *= gl_FragColor.a;
}
"
}
