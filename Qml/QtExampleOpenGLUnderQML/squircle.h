#pragma once
//此工程为Qt5.12的Demo修改，Qt5.15示例有一些变更
//变动1：
//在QQuickItem::releaseResources中
//调用window()->scheduleRenderJob()释放render
//之前的QQuickWindow::sceneGraphInvalidated逻辑还保留在，但是demo没触发信号
//变动2：
//绘制前调用window->beginExternalCommands()
//绘制后调用window->endExternalCommands()
//文档说是避免破坏状态
//
//示例安装路径：E:\Qt\QtOnline\Examples\Qt-5.15.2\quick\scenegraph\openglunderqml
//示例源码路径：E:\Qt\qt-everywhere-src-5.15.2\qtdeclarative\examples\quick\scenegraph\openglunderqml\openglunderqml.pro
//参考文档：https://doc.qt.io/qt-5/qquickitem.html
//参考文档：https://doc.qt.io/qt-5/qquickwindow.html
//参考文档：https://doc.qt.io/qt-5/qtquick-visualcanvas-scenegraph.html#scene-graph-and-rendering
//参考文档：https://doc.qt.io/qt-5/qtquick-scenegraph-openglunderqml-example.html
#include <QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>

//示例做了个简单封装，将渲染部分单独提出来
class SquircleRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    SquircleRenderer() {}
    ~SquircleRenderer();

    void setT(qreal t) { m_t = t; }
    void setViewport(const QRect &size) { m_viewport = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
    void paint();

private:
    QRect m_viewport;
    qreal m_t = 0;
    QOpenGLShaderProgram *m_program = nullptr;
    QQuickWindow *m_window = nullptr;
};

//方圆形(squircle)一词是方形(square)与圆形(circle)的融合
//比圆角矩形(roundrect)看起来更圆一点
class Squircle : public QQuickItem
{
    Q_OBJECT
    //自定义属性t，渲染时用于计算渐变色，UI中动画循环更新其值
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
public:
    Squircle();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t = 0;
    //绘制简单的封装到一个类中
    SquircleRenderer *m_renderer = nullptr;
};
