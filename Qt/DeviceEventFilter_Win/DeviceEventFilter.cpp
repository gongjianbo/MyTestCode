#include "DeviceEventFilter.h"
#include <QUuid>
#include <QDebug>
#include <Dbt.h>
#pragma comment(lib, "user32.lib")

GUID Hdr_Guid = { 0xAE18AA60, 0x7F6A, 0x11d4, { 0x97, 0xDD, 0x00, 0x01, 0x02, 0x29, 0xB9, 0x59 } };

DeviceEventFilter::DeviceEventFilter(QObject *parent)
    : QObject(parent)
{

}

DeviceEventFilter::~DeviceEventFilter()
{
    uninstallFilter();
}

void DeviceEventFilter::installFilter(HANDLE window)
{
    //初始化DEV_BROADCAST_DEVICEINTERFACE数据结构
    DEV_BROADCAST_DEVICEINTERFACE filter_data;
    memset(&filter_data, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    filter_data.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    filter_data.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter_data.dbcc_classguid = Hdr_Guid;
    devNotify = ::RegisterDeviceNotification(window, &filter_data, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void DeviceEventFilter::uninstallFilter()
{
    if (devNotify) {
        ::UnregisterDeviceNotification(devNotify);
        devNotify = NULL;
    }
}

bool DeviceEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        do {
            if (!msg || !msg->lParam || !msg->wParam || msg->message != WM_DEVICECHANGE)
                break;
            //PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
            //设备可用事件
            const bool is_add = msg->wParam == DBT_DEVICEARRIVAL;
            //设备移除事件
            const bool is_remove = msg->wParam == DBT_DEVICEREMOVECOMPLETE;
            if (!is_add && !is_remove)
                break;
            //过滤 device interface class 以外类型的消息
            DEV_BROADCAST_HDR *broadcast = (DEV_BROADCAST_HDR*)msg->lParam;
            if (!broadcast || broadcast->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
                break;
            //过滤不监听的设备类型
            DEV_BROADCAST_DEVICEINTERFACE *device_interface = (DEV_BROADCAST_DEVICEINTERFACE*)broadcast;
            if (!device_interface || device_interface->dbcc_classguid != Hdr_Guid)
                break;
            QUuid uid(device_interface->dbcc_classguid);
            qDebug()<<uid<<device_interface->dbcc_name<<"add"<<is_add<<"remove"<<is_remove;
            if (is_add) {
                emit deviceAdded();
            } else if (is_remove) {
                emit deviceRemoved();
            }
        } while(false);
    }
    return false;
}
