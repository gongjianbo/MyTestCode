#pragma once
#include <QObject>
#include <QQuickImageResponse>
#include <QQuickAsyncImageProvider>
#include <QImage>
#include <QTimer>
#include <QThreadPool>
#include <QRunnable>
#include <atomic>


// 参考：https://github.com/mixxxdj/mixxx.git 项目 src/qml/asyncimageprovider.h
class MyAsyncImageResponse : public QQuickImageResponse, public QRunnable
{
    Q_OBJECT
public:
    MyAsyncImageResponse(const QString &filepath, const QSize &requestedSize);
    ~MyAsyncImageResponse();

    // QRunnable 线程中加载图片
    void run() override;
    // Factory 提供接口从 QML 加载自定义纹理
    QQuickTextureFactory *textureFactory() const override;
    // 加载失败可以给报错提示
    QString errorString() const override;

public slots:
    // Image 没有引用该图了，还没返回就需要取消
    void cancel() override;

private:
    QString mFilepath;
    QSize mRequestedSize;
    QImage mImage;
    std::atomic_bool mRunning{true};
};

// 如果需要信号槽的话可以继承 QObject
class MyAsyncImageProvider : public QObject, public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    explicit MyAsyncImageProvider(QObject *parent = nullptr);

    // 通过该接口给 QML Image 提供数据
    // 文档注释：此方法可能由多个线程调用，因此请确保此方法的实现是可重入的
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

signals:
    // 测试刷新
    void imageChanged(const QString &imageTag);

private:
    // 加载图片的线程池
    QThreadPool pool;
    // 测试定时刷新
    QTimer *timer;
};
