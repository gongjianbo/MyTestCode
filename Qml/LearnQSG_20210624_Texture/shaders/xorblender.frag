uniform lowp float qt_Opacity;
uniform lowp sampler2D uSource1;
uniform lowp sampler2D uSource2;

varying highp vec2 vTexCoord;

void main()
{
    lowp vec4 p1 = texture2D(uSource1, vTexCoord);
    lowp vec4 p2 = texture2D(uSource2, vTexCoord);
    gl_FragColor = (p1 * (1.0 - p2.a) + p2 * (1.0 - p1.a)) * qt_Opacity;
}
