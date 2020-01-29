#include "FBORenderer.h"

#include <QDebug>

FBORenderer::FBORenderer()
{
    doInitialize();
}

void FBORenderer::render()
{
    doRender();
    //不知道为什么示例在这里加了update，加了在缩放的时候明显闪烁感更强
    //update();
}

QOpenGLFramebufferObject *FBORenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void FBORenderer::doInitialize()
{
    //为当前上下文初始化OpenGL函数解析
    initializeOpenGLFunctions();

    //着色器代码
    //in输入，out输出,uniform从cpu向gpu发送
    const char *vertex_str=R"(#version 330 core
                           layout (location = 0) in vec3 vertices;
                           void main() {
                           gl_Position = vec4(vertices,1.0);
                           })";
    const char *fragment_str=R"(#version 330 core
                             uniform vec3 myVar;
                             void main() {
                             gl_FragColor = vec4(myVar,1.0);
                             })";

    //将source编译为指定类型的着色器，并添加到此着色器程序
    if(!_program.addCacheableShaderFromSourceCode(
                QOpenGLShader::Vertex,vertex_str)){
        qDebug()<<"compiler vertex error";
        exit(0);
    }
    //界面定义了变量myVar，将在程序中设定这个变量
    if(!_program.addCacheableShaderFromSourceCode(
                QOpenGLShader::Fragment,fragment_str)){
        qDebug()<<"compiler fragment error";
        exit(0);
    }
    //使用addShader()将添加到该程序的着色器链接在一起。
    _program.link();

    //将属性名称绑定到指定位置(这里location=0)
    _program.bindAttributeLocation("vertices", 0);
}

void FBORenderer::doRender()
{
    //【1】
    //启用或禁用写入深度缓冲区
    glDepthMask(true);

    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glFrontFace()是opengl的初级命令，有两个基本作用:
    //一是可以用来用在某些特殊场合（比如剔除面片），二是可以提高渲染效率。
    //GL_CCW 表示窗口坐标上投影多边形的顶点顺序为逆时针方向的表面为正面。(默认值)
    //GL_CW 表示顶点顺序为顺时针方向的表面为正面。
    glFrontFace(GL_CW);
    //glCullFace()参数包括GL_FRONT和GL_BACK。
    //分别表示禁用多边形正面或者背面上的光照、阴影和颜色计算及操作，消除不必要的渲染计算。
    //glCullFace(GL_FRONT);
    //开启剔除操作效果(见glCullFace命令)
    //glEnable(GL_CULL_FACE);
    //当我们需要绘制透明图片时，就需要关闭GL_DEPTH_TEST并且打开混合glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    //【2】
    //将此着色器程序绑定到活动的QOpenGLContext，并使其成为当前的着色器程序
    //同于调用glUseProgram(programid)
    _program.bind();
    //传递值
    _program.setUniformValue("myVar", QVector3D(0,1,0));

    //不知道为什么再FBO框架下，使用glBindVertexArray(VAO)缩放之后图像就没了
    //Qt默认是和OpenGL里颠倒过来的，上负下正，
    //但是可以在QQuickFramebufferObject设置setMirrorVertically(true);
    float vertices[] = {
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f,-0.5f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f   // top
    };
    //在此着色器程序中的位置处启用顶点数组，
    //以便着色器程序将使用在位置上由setAttributeArray()设置的值。
    _program.enableAttributeArray(0);
    //给对应位置设置顶点数组
    _program.setAttributeArray(0, GL_FLOAT, vertices, 3);
    //从数组数据渲染基元(render primitives from array data)
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _program.disableAttributeArray(0);

    //从当前QOpenGLContext释放活动的着色器程序
    //相当于调用glUseProgram(0)
    _program.release();

    //【3】
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
}
