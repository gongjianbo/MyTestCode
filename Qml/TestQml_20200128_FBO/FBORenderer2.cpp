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
    glClear(GL_COLOR_BUFFER_BIT);

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
    //_program.bindAttributeLocation("vertices", 0);

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
                                0, 1, 3,  // first Triangle
                                1, 2, 3   // second Triangle
                             };
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
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
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
}

void FBORenderer2::doRender()
{
    //将此着色器程序绑定到活动的QOpenGLContext，并使其成为当前的着色器程序
    //同于调用glUseProgram(programid)
    _program.bind();
    //传递值
    _program.setUniformValue("myVar", QVector3D(0,1,0));

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //从当前QOpenGLContext释放活动的着色器程序
    //相当于调用glUseProgram(0)
    _program.release();
}
