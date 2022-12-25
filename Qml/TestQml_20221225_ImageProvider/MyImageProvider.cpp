#include "MyImageProvider.h"

MyImageProvider::MyImageProvider(QObject *parent)
    : QObject{parent}
    , QQuickImageProvider{QQuickImageProvider::Image}
    , timer{new QTimer(this)}
{
    //懒得找图片，就用纯色的QImage
    image = QImage(200, 200, QImage::Format_ARGB32);
    image.fill(QColor(gray, gray, gray));

    connect(timer, &QTimer::timeout, this, [this](){
        gray++;
        image.fill(QColor(gray, gray, gray));
        emit imageChanged("imageTag");
    });
    timer->start(20);
}

QImage MyImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    //指定Image的sourceSize会作为requestedSize参数
    Q_UNUSED(requestedSize)
    //url为"image://Provider/imageTag/0"，那么id就是"imageTag/0"
    if (id.startsWith("imageTag")) {
        if (size) {
            *size = image.size();
        }
        return image;
    }
    //如果返回的QImage无效，qml会报错，不过不需要报错，可以准备一个透明的QImage
    return QImage();
}
