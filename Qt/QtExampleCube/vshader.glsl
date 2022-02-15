#ifdef GL_ES
//设置默认精度
precision mediump int;
precision mediump float;
#endif
//movel-view-projection矩阵计算后的值
uniform mat4 mvp_matrix;
//顶点坐标属性
attribute vec4 a_position;
//纹理坐标属性
attribute vec2 a_texcoord;
//纹理坐标传递到图元着色器
varying vec2 v_texcoord;

void main()
{
    //计算顶点在屏幕空间的位置
    gl_Position = mvp_matrix * a_position;
    //纹理坐标传递到图元着色器
    v_texcoord = a_texcoord;
}
