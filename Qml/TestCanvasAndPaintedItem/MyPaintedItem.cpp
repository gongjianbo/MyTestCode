#include "MyPaintedItem.h"
#include <math.h>
#include <QtMath>
#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QDebug>

MyPaintedItem::MyPaintedItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{

}

void MyPaintedItem::paint(QPainter *painter)
{
    qDebug() << "cpp begin" << QTime::currentTime();
    QRect rect = boundingRect().toRect();
    QPoint center = rect.center();

    painter->save();
    painter->translate(center.x(), center.y());
    painter->setRenderHint(QPainter::Antialiasing, true);

    int r = qMin(rect.width(), rect.height()) / 2 - 2;
    double line_width = 0.5; // + 1E-7;
    QPen pen;
    pen.setColor(QColor(127, 0, 0));
    pen.setWidthF(line_width);
    pen.setJoinStyle(Qt::MiterJoin);
    // QML Canvas 默认 flat, QPainter 默认 square
    // 如果不使用 flat，多次旋转后可能会不均匀
    // pen.setCapStyle(Qt::FlatCap);
    for (int i = 0; i < mLineCount; i++)
    {
        // 如果不使用 save/restore, 多次旋转后可能会不均匀
        painter->save();
        painter->rotate(mRotation + 360.0 / mLineCount * i);
        QPainterPath path;
        path.moveTo(0, 0);
        path.lineTo(r, 0);
        painter->strokePath(path, pen);
        painter->restore();
    }
    painter->restore();
    qDebug() << "cpp end" << QTime::currentTime();
}

void MyPaintedItem::rotate(double rotation)
{
    mRotation = rotation;
    update();
}

void MyPaintedItem::resize(int count)
{
    mLineCount = count;
    update();
}
