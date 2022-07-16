#pragma once
#include <QQuickItem>

//学习QSGTextureMaterial的基本使用
class MyTextureItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *source READ getSource WRITE setSource NOTIFY sourceChanged)
public:
    explicit MyTextureItem(QQuickItem * parent = nullptr);

    //设置提供纹理的item
    QQuickItem *getSource() const { return source; }
    void setSource(QQuickItem *item);

protected:
    //更新渲染节点时调用
    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

signals:
    void sourceChanged();

private:
    //提供纹理的对象
    QQuickItem *source = nullptr;
    //变更source后需要重置texture
    bool sourceChange = false;
};
