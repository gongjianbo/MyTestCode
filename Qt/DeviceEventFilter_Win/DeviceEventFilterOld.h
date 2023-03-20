#pragma once
#include <QObject>
#include <QHash>
#include <QUuid>
#include <QAbstractNativeEventFilter>
#include <Windows.h>

/**
 * @brief 设备插拔事件监听
 * @author 龚建波
 * @date 2022-12-24
 * @details
 * 设备类文档，可以在设备管理器找自己的设备guid
 * https://learn.microsoft.com/zh-cn/windows-hardware/drivers/install/overview-of-device-setup-classes
 */
class DeviceEventFilterOld : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit DeviceEventFilterOld(QObject *parent = nullptr);
    ~DeviceEventFilterOld();

    // 注册设备guid关联窗口id，RegisterDeviceNotification
    // 暂未考虑注册失败的处理
    // 暂未考虑重复注册
    void installFilter(HANDLE winId, const QVector<QUuid> &uuids);
    // UnregisterDeviceNotification
    void uninstallFilter();

    // QAbstractNativeEventFilter回调
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

signals:
    void deviceAdded();
    void deviceRemoved();

private:
    // 设备通知句柄和uuid
    QHash<QUuid, HDEVNOTIFY> devNotifys;
};
