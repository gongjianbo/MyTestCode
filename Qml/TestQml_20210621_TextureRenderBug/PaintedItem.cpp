#include "PaintedItem.h"

#include <QPainter>

void PaintedItem::paint(QPainter *painter)
{
    QRect rect(0,0,width(),height());
    painter->fillRect(rect,Qt::black);
    painter->setPen(Qt::white);
    painter->drawText(rect,"PaintedItem\nTest Ok?");
}
