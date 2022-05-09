#include "FBORenderer.h"

#include <QDebug>

FBORenderer::FBORenderer()
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
                             uniform vec3 myColor;
                             void main() {
                             gl_FragColor = vec4(myColor,1.0);
                             })";

    //将source编译为指定类型的着色器，并添加到此着色器程序
    if(!program.addCacheableShaderFromSourceCode(
                QOpenGLShader::Vertex,vertex_str)){
        qDebug()<<"compiler vertex error";
        exit(0);
    }
    if(!program.addCacheableShaderFromSourceCode(
                QOpenGLShader::Fragment,fragment_str)){
        qDebug()<<"compiler fragment error";
        exit(0);
    }
    //使用addShader()将添加到该程序的着色器链接在一起。
    program.link();

    //将属性名称绑定到指定位置(这里location=0)
    program.bindAttributeLocation("vertices", 0);
}

void FBORenderer::render()
{
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    //create FBO 时附加了深度缓冲
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //将此着色器程序绑定到活动的QOpenGLContext，并使其成为当前的着色器程序
    //同于调用glUseProgram(programid)
    program.bind();
    //传递值
    program.setUniformValue("myColor", QVector3D(0,1,0));

    //Qt默认是和OpenGL里颠倒过来的，上负下正，
    //但是可以在QQuickFramebufferObject设置setMirrorVertically(true);
    float vertices[] = {
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f,-0.5f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f   // top
    };
    //在此着色器程序中的位置处启用顶点数组，
    //以便着色器程序将使用在位置上由setAttributeArray()设置的值。
    program.enableAttributeArray(0);
    //给对应位置设置顶点数组
    program.setAttributeArray(0, GL_FLOAT, vertices, 3);
    //从数组数据渲染图元(render primitives from array data)
    glDrawArrays(GL_TRIANGLES, 0, 3);
    program.disableAttributeArray(0);

    //从当前QOpenGLContext释放活动的着色器程序
    //相当于调用glUseProgram(0)
    program.release();
}

QOpenGLFramebufferObject *FBORenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    //attach后需要开启GL_DEPTH_TEST
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}
