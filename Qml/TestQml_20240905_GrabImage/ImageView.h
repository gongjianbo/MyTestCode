#pragma once
#include <QQuickWindow>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>

// QML 没法直接设置 QImage，写个简单的 Item 绘制
class ImageView : public QQuickPaintedItem
{
    Q_OBJECT
public:
    using QQuickPaintedItem::QQuickPaintedItem;

    Q_INVOKABLE void grabWindow(QQuickWindow *window);
    Q_INVOKABLE void loadImage(const QImage &image);

    void paint(QPainter *painter) override;

private:
    QImage curImage;
};

