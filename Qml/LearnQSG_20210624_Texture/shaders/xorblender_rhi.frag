#version 440

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
} ubuf;

layout(binding = 1) uniform sampler2D uSource1;
layout(binding = 2) uniform sampler2D uSource2;

void main()
{
    lowp vec4 p1 = texture(uSource1, vTexCoord);
    lowp vec4 p2 = texture(uSource2, vTexCoord);
    fragColor = (p1 * (1.0 - p2.a) + p2 * (1.0 - p1.a)) * ubuf.qt_Opacity;
}
