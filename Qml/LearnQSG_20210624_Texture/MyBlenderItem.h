#pragma once
#include <QQuickItem>

//复制twotextureproviders示例
class MyBlenderItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *source1 READ getSource1 WRITE setSource1 NOTIFY source1Changed)
    Q_PROPERTY(QQuickItem *source2 READ getSource2 WRITE setSource2 NOTIFY source2Changed)
public:
    explicit MyBlenderItem(QQuickItem * parent = nullptr);

    QQuickItem *getSource1() const { return source1; }
    void setSource1(QQuickItem *item);

    QQuickItem *getSource2() const { return source2; }
    void setSource2(QQuickItem *item);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

signals:
    void source1Changed();
    void source2Changed();

private:
    QQuickItem *source1 = nullptr;
    bool source1Change = false;

    QQuickItem *source2 = nullptr;
    bool source2Change = false;
};
