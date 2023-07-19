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
    // QPainter 绘制的没 Canvas 均匀，加一点小数效果好一线
    double line_width = 1.0 + 1E-7;
    painter->rotate(mRotation);
    for (int i = 0; i < mLineCount; i++)
    {
        painter->setPen(QPen(QColor(255.0 * i / mLineCount, 0, 0), line_width));
        painter->drawLine(0, 0, r, 0);
        painter->rotate(360.0 / mLineCount);
    }
    painter->restore();
    qDebug() << "cpp end" << QTime::currentTime();
}

void MyPaintedItem::rotate(double rotation)
{
    mRotation = rotation;
    update();
}
