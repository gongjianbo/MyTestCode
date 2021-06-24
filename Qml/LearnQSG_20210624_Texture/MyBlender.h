#ifndef MYBLENDER_H
#define MYBLENDER_H

#include <QQuickItem>

class MyBlender : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *source READ getSource WRITE setSource NOTIFY sourceChanged)
public:
    explicit MyBlender(QQuickItem * parent = nullptr);

    QQuickItem *getSource() const { return source; }
    void setSource(QQuickItem *item);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

signals:
    void sourceChanged();

private:
    QQuickItem *source = nullptr;
    bool sourceChange = false;
};

#endif // MYBLENDER_H
