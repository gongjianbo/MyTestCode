#pragma once
#include "geometryengine.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

//管理顶点缓冲
class GeometryEngine;

//QOpenGLWidget窗口上下文，作用同GLFW库
//QOpenGLFunctions访问OpenGL接口，可以不继承作为成员变量使用，作用同GLAD库
class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    //继承构造函数
    using QOpenGLWidget::QOpenGLWidget;
    //析构释放纹理和顶点缓冲
    ~MainWidget();

protected:
    //release-press获取到鼠标拖动的方向
    //配合四元数计算出物体旋转的向量
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    //物体旋转时累减偏移量实现动画效果
    void timerEvent(QTimerEvent *e) override;

    //【】继承QOpenGLWidget后重写这三个虚函数
    //设置OpenGL资源和状态。在第一次调用resizeGL或paintGL之前被调用一次
    void initializeGL() override;
    //设置OpenGL视口、投影等，每当尺寸大小改变时调用
    void resizeGL(int w, int h) override;
    //渲染OpenGL场景，每当需要更新小部件时使用
    void paintGL() override;

    //初始化着色器
    void initShaders();
    //初始化纹理
    void initTextures();

private:
    //定时器，配合timerEvent使用
    QBasicTimer timer;
    //Qt着色器类封装
    QOpenGLShaderProgram program;
    //管理顶点缓冲，内部使用QOpenGLBuffer
    GeometryEngine *geometries = nullptr;
    //Qt纹理类封装
    QOpenGLTexture *texture = nullptr;
    //投影矩阵，resizeGL时更新，使最终渲染长宽保持比例
    QMatrix4x4 projection;

    //鼠标按下时坐标
    QVector2D mousePressPosition;
    //物体拖动旋转轴
    QVector3D rotationAxis;
    //动画步进
    qreal angularSpeed = 0;
    //四元数，记录物体叠加的旋转状态
    //通过rotationAxis和angularSpeed来更新
    QQuaternion rotation;
};
