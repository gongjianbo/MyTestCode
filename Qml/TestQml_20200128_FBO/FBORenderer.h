#ifndef FBORENDERER_H
#define FBORENDERER_H

#include <QtQuick/QQuickFramebufferObject>

//封装一个OpenGL帧缓冲区对象
#include <QtGui/QOpenGLFramebufferObject>

//代表了本地的OpenGL背景下，支持在OpenGL渲染QSurface
#include <QtGui/QOpenGLContext>

//允许OpenGL着色程序链接和使用
#include <QtGui/QOpenGLShaderProgram>

//提供了跨平台访问的OpenGL ES 2.0 API
//Qt Quick2.0使用专用的基于OpenGL ES2.0的Qt Quick Scene Graph场景图进行所有渲染
//#include <QtGui/QOpenGLFunctions>

//提供OpenGL 3.3核心配置文件
#include <QtGui/QOpenGLFunctions_3_3_Core>

class FBORenderer
        : public QQuickFramebufferObject::Renderer,
        protected QOpenGLFunctions_3_3_Core
{
public:
    FBORenderer();
    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    void doInitialize();
    void doRender();

private:
    //着色器程序
    QOpenGLShaderProgram _program;
};

#endif // FBORENDERER_H
