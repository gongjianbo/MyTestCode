#include "MyImageProvider.h"

MyImageProvider::MyImageProvider(QObject *parent)
    : QObject{parent}
    , QQuickImageProvider{QQuickImageProvider::Image}
    , timer{new QTimer(this)}
{
    // 生成一个 QImage 来测试
    image = QImage(200, 200, QImage::Format_ARGB32);
    image.fill(QColor(gray, gray, gray));

    // 测试刷新
    connect(timer, &QTimer::timeout, this, [this](){
        gray++;
        image.fill(QColor(gray, gray, gray));
        emit imageChanged("imageTag");
    });
    timer->start(20);
}

QImage MyImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    // 指定 Image 的 sourceSize 会作为 requestedSize 参数，默认是 QSize(-1, -1)
    Q_UNUSED(requestedSize)
    // 若 url 为 "image://Provider/imageTag/0"，那么 id 就是 "imageTag/0"
    // 注意 url 的编码问题，如果有特殊符号之类的不能直接从中截取 QML 中设置的路径
    if (id.startsWith("imageTag")) {
        if (size) {
            *size = image.size();
        }
        return image;
    }
    // 如果返回的 QImage 无效，QML 会报错，不过不需要报错，可以准备一个透明的 QImage
    return QImage();
}
