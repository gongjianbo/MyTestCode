#pragma once
#include <QQuickPaintedItem>

class MyPaintedItem : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit MyPaintedItem(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

    Q_INVOKABLE void rotate(double rotation);

private:
    int mLineCount{3600};
    double mRotation{0.0};
};
