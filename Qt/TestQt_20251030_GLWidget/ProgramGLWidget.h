#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QImage>

// 测试可编程管线
class ProgramGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ProgramGLWidget(QWidget *parent = nullptr);
    ~ProgramGLWidget();

    void setImage(const QImage &image);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void updateTexture();
    void updateProjection(int viewW, int viewH, int imageW, int imageH);

private:
    // 着色器程序
    QOpenGLShaderProgram shaderProgram;
    // 顶点数组对象
    QOpenGLVertexArrayObject vao;
    // 顶点缓冲
    QOpenGLBuffer vbo;
    // 索引缓冲
    QOpenGLBuffer ebo;
    // 纹理
    QOpenGLTexture *texture{ nullptr };
    // 投影矩阵
    QMatrix4x4 projection;
    // 纹理图像
    QImage cache;
};
