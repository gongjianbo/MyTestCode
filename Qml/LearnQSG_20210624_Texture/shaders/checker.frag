uniform lowp vec4 color1;
uniform lowp vec4 color2;
uniform highp vec2 pixelSize;

varying highp vec2 qt_TexCoord0;

void main()
{
    highp vec2 tc = sign(sin(3.14159265358979323846 * qt_TexCoord0 * pixelSize));
    if (tc.x != tc.y)
        gl_FragColor = color1;
    else
        gl_FragColor = color2;
}
