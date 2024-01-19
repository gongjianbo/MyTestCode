#pragma once
#include <QObject>
#include <QQuickImageProvider>
#include <QImage>
#include <QTimer>

// 参考文档：https://doc.qt.io/qt-6/qquickimageprovider.html
class MyImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit MyImageProvider(QObject *parent = nullptr);

    // 通过该接口给 QML Image 提供数据
    // 文档注释：此方法可能由多个线程调用，因此请确保此方法的实现是可重入的
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    // 其他的接口
    // QPixmap requestPixmap(const QString &id, QSize *size, const QSize& requestedSize) override;
    // QQuickTextureFactory *requestTexture(const QString &id, QSize *size, const QSize &requestedSize) override;

signals:
    // 测试刷新
    void imageChanged(const QString &imageTag);

private:
    // 测试定时刷新
    QTimer *timer;
    // 颜色
    uchar gray{0};
    // 表示内存中的图
    QImage image;
};
