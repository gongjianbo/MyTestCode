#include "QmlPainter.h"

#include <QDebug>

QmlPainter::QmlPainter(QPainter *painter)
    :painter(painter)
{
    //qDebug()<<"init";
}

QmlPainter::~QmlPainter()
{
    //qDebug()<<"free";
}

void QmlPainter::drawLine(int x1, int y1, int x2, int y2)
{
    painter->drawLine(x1,y1,x2,y2);
}
