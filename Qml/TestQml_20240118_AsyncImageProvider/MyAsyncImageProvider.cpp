#include "MyAsyncImageProvider.h"
#include <QImageReader>
#include <QThread>
#include <QDebug>

MyAsyncImageResponse::MyAsyncImageResponse(const QString &filepath, const QSize &requestedSize)
    : mFilepath(filepath)
    , mRequestedSize(requestedSize)
{
    setAutoDelete(false);
    qDebug()<<__FUNCTION__<<QTime::currentTime();
}

MyAsyncImageResponse::~MyAsyncImageResponse()
{
    qDebug()<<__FUNCTION__;
}

void MyAsyncImageResponse::run()
{
    do {
        // 测试用延时
        QThread::msleep(15);
        if (mFilepath.isEmpty())
            break;
        QImageReader reader(this->mFilepath);
        if (!reader.canRead())
            break;
        mImage = reader.read();
        // 只缩小，不放大
        bool do_scale = mRequestedSize.width() < mImage.size().width() &&
                        mRequestedSize.height() < mImage.size().height();
        if (!mImage.isNull() && mRequestedSize.isValid() && do_scale) {
            // 如果用 QML 中的缩放设置，就不在这里进行缩放了
            // Qt::FastTransformation
            mImage = mImage.scaled(mRequestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    while(false);
    // 在父类构造函数关联，发出之后才会进入下一步
    emit finished();
}

QQuickTextureFactory *MyAsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(mImage);
}

QString MyAsyncImageResponse::errorString() const
{
    if (!mRunning && mImage.isNull()) {
        return "Image Load Error";
    }
    return "";
}

void MyAsyncImageResponse::cancel()
{
    // 这里设置 run 的退出条件
    qDebug()<<__FUNCTION__;
}

MyAsyncImageProvider::MyAsyncImageProvider(QObject *parent)
    : QObject{parent}
    , timer{new QTimer(this)}
{
    pool.setMaxThreadCount(8);

    // 测试刷新
    connect(timer, &QTimer::timeout, this, [this](){
        emit imageChanged("imageTag");
    });
    timer->start(20);
}

QQuickImageResponse *MyAsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    // 指定 Image 的 sourceSize 会作为 requestedSize 参数，默认是 QSize(-1, -1)
    // 若 url 为 "image://Provider/imageTag/0"，那么 id 就是 "imageTag/0"
    // 注意 url 的编码问题，如果有特殊符号之类的不能直接从中截取 QML 中设置的路径
    // id 到实际路径的转换根据实际需求来，此处取固定值
    Q_UNUSED(id)
    QString path = ":/flower.jpg";
    auto response = new MyAsyncImageResponse(path, requestedSize);
    pool.start(response);
    return response;
}
