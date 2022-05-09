#include "FBORenderer2.h"

#include <QDebug>

FBORenderer2::FBORenderer2()
{
    doInitialize();
}

FBORenderer2::~FBORenderer2()
{
    doFree();
}

void FBORenderer2::render()
{
    glClearColor(0.1f, 0.3f, 0.2f, 1.0f);
    //create FBO 时附加了深度缓冲
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //渲染
    doRender();
}

QOpenGLFramebufferObject *FBORenderer2::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void FBORenderer2::doInitialize()
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
    //program.bindAttributeLocation("vertices", 0);

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FBORenderer2::doFree()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void FBORenderer2::doRender()
{
    //将此着色器程序绑定到活动的QOpenGLContext，并使其成为当前的着色器程序
    //同于调用glUseProgram(programid)
    program.bind();
    //传递值
    program.setUniformValue("myColor", QVector3D(0,1,0));

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //从当前QOpenGLContext释放活动的着色器程序
    //相当于调用glUseProgram(0)
    program.release();
}
