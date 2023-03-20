#pragma once
#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include <Windows.h>
class DeviceEventFilterPrivate;

/**
 * @brief 设备插拔事件监听
 * @author 龚建波
 * @date 2022-12-24
 * @details
 * 设备类文档，可以在设备管理器找自己的设备 GUID
 * https://learn.microsoft.com/zh-cn/windows-hardware/drivers/install/overview-of-device-setup-classes
 * @history
 * 2023-03-20
 * 参考 https://github.com/wang-bin/qdevicewatcher
 * 由 QAbstractNativeEventFilter 过滤事件改为了创建一个 win32 窗口来接收事件
 */
class DeviceEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit DeviceEventFilter(QObject *parent = nullptr);
    ~DeviceEventFilter();

    // RegisterDeviceNotification 注册对应的 GUID 消息通知
    // 暂未考虑重复注册和注册失败的处理
    void init(const QVector<QUuid> &uuids);

    // UnregisterDeviceNotification
    // 会在析构中自动调用一次
    void free();

signals:
    // 设备插入
    void deviceAttached(quint16 vid, quint16 pid);
    // 设备拔出
    void deviceDetached(quint16 vid, quint16 pid);

private:
    friend class DeviceEventFilterPrivate;
    QSharedPointer<DeviceEventFilterPrivate> dptr;
};
