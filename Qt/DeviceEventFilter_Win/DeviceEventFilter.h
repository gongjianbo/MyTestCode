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

    //注册设备guid关联窗口id，RegisterDeviceNotification
    void installFilter(HANDLE winId);
    //UnregisterDeviceNotification
    void uninstallFilter();

    //QAbstractNativeEventFilter回调
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

signals:
    void deviceAdded();
    void deviceRemoved();

private:
    //设备通知句柄
    HDEVNOTIFY devNotify{NULL};
};
