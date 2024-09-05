#include "ImageView.h"
#include <QDebug>

void ImageView::grabWindow(QQuickWindow *window)
{
    loadImage(window->grabWindow());
}

void ImageView::loadImage(const QImage &image)
{
    qDebug() << __FUNCTION__ << image;
    curImage = image;
    update();
}

void ImageView::paint(QPainter *painter)
{
    painter->fillRect(boundingRect(), Qt::black);
    painter->drawImage(boundingRect().adjusted(10, 10, -10, -10), curImage);
}
