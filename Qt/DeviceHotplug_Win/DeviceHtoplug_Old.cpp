#include "DeviceHtoplug_Old.h"
#include <QDebug>
#include <Dbt.h>
#pragma comment(lib, "user32.lib")

DeviceHotplug_Old::DeviceHotplug_Old(QObject *parent)
    : QObject(parent)
{

}

DeviceHotplug_Old::~DeviceHotplug_Old()
{
    uninstallFilter();
}

void DeviceHotplug_Old::installFilter(HANDLE winId, const QVector<QUuid> &uuids)
{
    if (!winId)
        return;
    // 初始化DEV_BROADCAST_DEVICEINTERFACE数据结构
    DEV_BROADCAST_DEVICEINTERFACE filter_data;
    memset(&filter_data, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    filter_data.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    filter_data.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    for (auto &&uuid : uuids)
    {
        filter_data.dbcc_classguid = uuid;
        HDEVNOTIFY handle = ::RegisterDeviceNotification(winId, &filter_data, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (handle) {
            devNotifys.insert(uuid, handle);
        } else {
            qDebug()<<"RegisterDeviceNotification error"<<uuid;
        }
    }
}

void DeviceHotplug_Old::uninstallFilter()
{
    for (HDEVNOTIFY handle : qAsConst(devNotifys))
    {
        ::UnregisterDeviceNotification(handle);
    }
    devNotifys.clear();
}

bool DeviceHotplug_Old::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        do {
            if (!msg || !msg->lParam || !msg->wParam || msg->message != WM_DEVICECHANGE)
                break;
            // 设备可用事件
            const bool is_add = msg->wParam == DBT_DEVICEARRIVAL;
            // 设备移除事件
            const bool is_remove = msg->wParam == DBT_DEVICEREMOVECOMPLETE;
            if (!is_add && !is_remove)
                break;
            // 过滤 device interface class 以外类型的消息
            DEV_BROADCAST_HDR *broadcast = (DEV_BROADCAST_HDR*)msg->lParam;
            if (!broadcast || broadcast->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
                break;
            // 过滤不监听的设备类型
            DEV_BROADCAST_DEVICEINTERFACE *device_interface = (DEV_BROADCAST_DEVICEINTERFACE*)broadcast;
            QUuid uid(device_interface->dbcc_classguid);
            if (!devNotifys.contains(uid))
                break;
            QString device_name;
            if (device_interface->dbcc_name) {
#ifdef UNICODE
                device_name = QString::fromWCharArray(device_interface->dbcc_name);
#else
                device_name = QString(device_interface->dbcc_name);
#endif
            }
            qDebug()<<uid<<"add"<<is_add<<"remove"<<is_remove<<device_name;
            if (is_add) {
                emit deviceAdded();
            } else if (is_remove) {
                emit deviceRemoved();
            }
        } while(false);
    }
    return false;
}
