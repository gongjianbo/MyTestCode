#pragma once
#include <QQuickItem>

class MyLine : public QQuickItem
{
    Q_OBJECT
public:
    explicit MyLine(QQuickItem *parent = nullptr);

    //渲染时会调用组件的此接口
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) override;
};

