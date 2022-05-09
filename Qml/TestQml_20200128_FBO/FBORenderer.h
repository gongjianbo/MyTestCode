#pragma once

#include <QtQuick/QQuickFramebufferObject>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLShaderProgram>
//#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLFunctions_3_3_Core>

//渲染相关放到 QQuickFramebufferObject::Renderer 子类
class FBORenderer : public QQuickFramebufferObject::Renderer,
        protected QOpenGLFunctions_3_3_Core
{
public:
    FBORenderer();
    //要渲染到 FBO，需要继承 Renderer 类并重新实现其 render() 函数
    void render() override;
    //创建新的 FBO 时调用，如 resize 时
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    //着色器程序
    QOpenGLShaderProgram program;
};
