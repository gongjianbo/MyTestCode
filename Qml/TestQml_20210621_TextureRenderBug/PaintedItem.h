#ifndef PAINTEDITEM_H
#define PAINTEDITEM_H

#include <QQuickPaintedItem>

class PaintedItem : public QQuickPaintedItem
{
    Q_OBJECT
public:
    using QQuickPaintedItem::QQuickPaintedItem;

    void paint(QPainter *painter) override;
};

#endif // PAINTEDITEM_H
