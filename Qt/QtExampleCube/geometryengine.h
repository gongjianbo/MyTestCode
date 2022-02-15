#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

//此类用于初始化和使用顶点和索引
//QOpenGLFunctions访问OpenGL接口
class GeometryEngine : protected QOpenGLFunctions
{
public:
    //在initializeGL中构造该对象，初始化顶点缓冲
    GeometryEngine();
    //释放顶点缓冲
    virtual ~GeometryEngine();
    //paintGL中调用，渲染顶点
    void drawCubeGeometry(QOpenGLShaderProgram *program);

private:
    //初始化顶点缓冲
    void initCubeGeometry();
    //vbo顶点缓冲，默认为QOpenGLBuffer::VertexBuffer
    QOpenGLBuffer arrayBuf;
    //ebo索引缓冲，在初始化列表中设置为QOpenGLBuffer::IndexBuffer
    QOpenGLBuffer indexBuf;
};

