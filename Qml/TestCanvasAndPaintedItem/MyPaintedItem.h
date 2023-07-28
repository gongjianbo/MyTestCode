#pragma once
#include <QQuickPaintedItem>

class MyPaintedItem : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit MyPaintedItem(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

    Q_INVOKABLE void rotate(double rotation);
    Q_INVOKABLE void resize(int count);

private:
    int mLineCount{360 * 4};
    double mRotation{0.0};
};
