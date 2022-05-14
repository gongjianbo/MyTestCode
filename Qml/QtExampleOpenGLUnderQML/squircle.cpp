#include "squircle.h"

#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QDebug>
#include <QThread>

SquircleRenderer::~SquircleRenderer()
{
    if (m_program) {
        delete m_program;
        m_program = nullptr;
    }
}

//关联信号QQuickWindow::before/afterRendering绘制
void SquircleRenderer::paint()
{
    if (!m_window) {
        return;
    }
    //示例绘制了一个简单的squircle渐变图案
    //在此基础上我加了一个glScissor只绘制Item所在区域，避免绘到整个Window
    if (!m_program) {
        //初始化OpenGL上下文
        initializeOpenGLFunctions();
        //初始化着色器程序
        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(
                    QOpenGLShader::Vertex,
                    "attribute highp vec4 vertices;"
                    "varying highp vec2 coords;"
                    "void main() {"
                    "    gl_Position = vertices;"
                    "    coords = vertices.xy;"
                    "}");
        m_program->addCacheableShaderFromSourceCode(
                    QOpenGLShader::Fragment,
                    "uniform lowp float t;"
                    "varying highp vec2 coords;"
                    "void main() {"
                    "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
                    "    i = smoothstep(t - 0.8, t + 0.8, i);"
                    "    i = floor(i * 20.) / 20.;"
                    "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
                    "}");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();
    }

    m_program->bind();
    m_program->enableAttributeArray(0);

    float values[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };
    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    m_program->setUniformValue("t", (float) m_t);

    int w = m_viewport.width();
    int h = m_viewport.height();
    int x = m_viewport.x();
    int y = m_viewport.y();
    glViewport(x, y , w, h);
    //只绘制Item所在区域，避免绘到整个Window
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y , w, h);

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    //两个三角的三角带，即一个矩形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_SCISSOR_TEST);

    m_program->disableAttributeArray(0);
    m_program->release();

    //调用此函数将 OpenGL 上下文重置为默认值/状态
    //此函数不会触及固定函数管道中的状态
    //此函数不会清除颜色、深度和模板缓冲区
    //使用QQuickWindow::setClearBeforeRendering来控制颜色缓冲区的清除
    //场景图渲染器可能会破坏深度和模板缓冲区，按需手动清除这些
    m_window->resetOpenGLState();
}

Squircle::Squircle()
{
    //itemChange函数中触发windowChanged
    //此示例仅在item添加到Window和移除时触发
    connect(this, &QQuickItem::windowChanged, this, &Squircle::handleWindowChanged);
}

void Squircle::setT(qreal t)
{
    if (t == m_t) {
        return;
    }
    m_t = t;
    emit tChanged();
    //UI动画更新属性值后，调用update刷新
    if (window()) {
        window()->update();
    }
}

//关联信号QQuickWindow::before/afterSynchronizing同步状态
void Squircle::sync()
{
    //第一次调用时进行初始化
    if (!m_renderer) {
        m_renderer = new SquircleRenderer();
        //设置了win->setClearBeforeRendering(false)的话beforeRendering时绘制的就被清掉了
        //connect(window(), &QQuickWindow::beforeRendering, m_renderer, &SquircleRenderer::paint, Qt::DirectConnection);
        //scene graph（QML场景图） render之后，swapbuffers前触发，在渲染线程
        //浮在QML已绘制的最上层
        connect(window(), &QQuickWindow::afterRendering, m_renderer, &SquircleRenderer::paint, Qt::DirectConnection);
    }
    //同步 UI 状态给 render ... ...
    //设置Item在Window中的位置，对应OpenGL的viewport以左下角为起点
    //示例是全屏，我这里设置为一个小方块
    m_renderer->setViewport(QRect(x(), window()->height() - y() - height(), width(), height()));
    m_renderer->setT(m_t);
    //render类中调用了window的接口
    m_renderer->setWindow(window());
}

//关联信号QQuickWindow::sceneGraphInvalidated释放
void Squircle::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

//关联信号QQuickItem::windowChanged
void Squircle::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        //因为UI线程和渲染线程可能不是同一个，所以Qt::DirectConnection连接，使在渲染线程绘制
        //render之前同步状态，在渲染线程
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Squircle::sync, Qt::DirectConnection);
        //render之后同步状态，在渲染线程
        //connect(win, &QQuickWindow::afterSynchronizing, this, &Squircle::sync, Qt::DirectConnection);
        //释放，在渲染线程
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Squircle::cleanup, Qt::DirectConnection);
        //render前clear
        //如果设置为false拖动窗口大小会有残影
        //而且QML Window的color就是clear填充的
        //如果设置为true则QQuickWindow::beforeRendering时绘制会被clear清掉
        //win->setClearBeforeRendering(false);
    }
}
