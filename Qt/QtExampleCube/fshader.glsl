#ifdef GL_ES
//设置默认精度
precision mediump int;
precision mediump float;
#endif
//纹理
uniform sampler2D texture;
//纹理坐标
varying vec2 v_texcoord;

void main()
{
    //获取对应位置纹理的颜色值
    gl_FragColor = texture2D(texture, v_texcoord);
}
