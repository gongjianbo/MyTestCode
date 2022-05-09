#pragma ocne

#include <QtQuick/QQuickFramebufferObject>

//QML UI 相关逻辑放到 QQuickFramebufferObject 子类
//渲染相关放到 QQuickFramebufferObject::Renderer 子类
//该类仅在 Qt Quick 通过 OpenGL 渲染时才起作用
class FBOItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    FBOItem(QQuickItem *parent = nullptr);

    //Renderer 实例是从 createRenderer() 返回的
    QQuickFramebufferObject::Renderer *createRenderer() const override;
};
