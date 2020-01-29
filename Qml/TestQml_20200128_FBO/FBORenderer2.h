#ifndef FBORENDERER2_H
#define FBORENDERER2_H

#include <QtQuick/QQuickFramebufferObject>
#include <QtGui/QOpenGLFramebufferObject>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions_3_3_Core>

class FBORenderer2
        : public QQuickFramebufferObject::Renderer,
        protected QOpenGLFunctions_3_3_Core
{
public:
    FBORenderer2();
    ~FBORenderer2();

    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    void doInitialize();
    void doFree();
    void doRender();

private:
    //着色器程序
    QOpenGLShaderProgram _program;
    //
    GLuint _VAO;
    GLuint _VBO;
    GLuint _EBO;
};

#endif // FBORENDERER2_H
