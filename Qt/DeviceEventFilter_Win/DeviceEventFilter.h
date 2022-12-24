#pragma once
#include <QObject>
#include <QAbstractNativeEventFilter>
#include <Windows.h>

/**
 * @brief 设备插拔事件监听
 * @author 龚建波
 * @date 2022-12-24
 */
class DeviceEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit DeviceEventFilter(QObject *parent = nullptr);
    ~DeviceEventFilter();

    //注册设备guid，windows这个事件接口需要关联窗口
    void installFilter(HANDLE window);
    void uninstallFilter();

    //事件回调
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

signals:
    void deviceAdded();
    void deviceRemoved();

private:
    //设备通知句柄
    HDEVNOTIFY devNotify{devNotify};
};
