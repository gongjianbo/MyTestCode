#pragma once
#include <QQuickItem>

class QSGLine : public QQuickItem
{
    Q_OBJECT
public:
    explicit QSGLine(QQuickItem *parent = nullptr);

    //渲染时会调用组件的此接口
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) override;
};

