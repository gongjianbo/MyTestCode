#include "DeviceHotplug.h"
#include <QMetaObject>
#include <QHash>
#include <QDebug>
#include <Dbt.h>
#pragma comment(lib, "user32.lib")

// 内部结构，存储对应平台需要的数据
class DeviceHotplugPrivate
{
public:
    void deviceAttached(quint16 vid, quint16 pid) {
        QMetaObject::invokeMethod(ptr, "deviceAttached", Qt::QueuedConnection,
                                  Q_ARG(quint16, vid),
                                  Q_ARG(quint16, pid));
    }
    void deviceDetached(quint16 vid, quint16 pid) {
        QMetaObject::invokeMethod(ptr, "deviceDetached", Qt::QueuedConnection,
                                  Q_ARG(quint16, vid),
                                  Q_ARG(quint16, pid));
    }
    // 处理窗口消息
    static LRESULT CALLBACK windowMessageProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    // win32 窗口类名
    static QString windowClassName();
    // 创建一个用于接收消息的窗口，注册消息回调
    bool createMessageWindow(const QVector<QUuid> &uuids);
    // 释放
    void destroyMessageWindow();

    // 关联的对象
    DeviceHotplug *ptr{nullptr};
    // 关联的窗口
    HWND hwnd{nullptr};
    // 设备通知句柄和 UUID/GUID
    QHash<QUuid, HDEVNOTIFY> devNotifys;
};

// 处理窗口消息
LRESULT CALLBACK DeviceHotplugPrivate::windowMessageProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DEVICECHANGE) {
        do {
            // 设备可用事件
            const bool is_add = wParam == DBT_DEVICEARRIVAL;
            // 设备移除事件
            const bool is_remove = wParam == DBT_DEVICEREMOVECOMPLETE;
            if (!is_add && !is_remove)
                break;
            // 过滤 device interface class 以外类型的消息
            DEV_BROADCAST_HDR *broadcast = reinterpret_cast<DEV_BROADCAST_HDR *>(lParam);
            if (!broadcast || broadcast->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
                break;
            // 获取 SetWindowLongPtrW 设置的对象
            DeviceHotplugPrivate *data = reinterpret_cast<DeviceHotplugPrivate *>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if (!data)
                break;
            // 过滤不监听的设备类型
            DEV_BROADCAST_DEVICEINTERFACE *device_interface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(broadcast);
            QUuid uid(device_interface->dbcc_classguid);
            if (!data->devNotifys.contains(uid))
                break;
            QString device_name;
            if (device_interface->dbcc_name) {
#ifdef UNICODE
                device_name = QString::fromWCharArray(device_interface->dbcc_name);
#else
                device_name = QString(device_interface->dbcc_name);
#endif
            }
            // 从设备描述中获取 vid 和 pid
            int offset = -1;
            quint16 vid = 0;
            quint16 pid = 0;
            offset = device_name.indexOf("VID_");
            if (offset > 0 && offset + 8 <= device_name.size()) {
                vid = device_name.mid(offset + 4, 4).toUShort(nullptr, 16);
            }
            offset = device_name.indexOf("PID_");
            if (offset > 0 && offset + 8 <= device_name.size()) {
                pid = device_name.mid(offset + 4, 4).toUShort(nullptr, 16);
            }
            if (is_add) {
                fprintf(stderr, "device attached: vid 0x%04x, pid 0x%04x.\n", vid, pid);
                data->deviceAttached(vid, pid);
            } else if (is_remove) {
                fprintf(stderr, "device detached: vid 0x%04x, pid 0x%04x.\n", vid, pid);
                data->deviceDetached(vid, pid);
            }
        } while(false);
    }

    return ::DefWindowProcW(hwnd, message, wParam, lParam);
}

// 窗口类名
QString DeviceHotplugPrivate::windowClassName()
{
    return QLatin1String("Qt_DeviceHotplug_Window_") + QString::number(quintptr(windowMessageProcess));
}

// 创建一个用于接收消息的窗口，注册消息回调
bool DeviceHotplugPrivate::createMessageWindow(const QVector<QUuid> &uuids)
{
    QString class_name = windowClassName();
    HINSTANCE hi = ::GetModuleHandleW(nullptr);

    WNDCLASSW wc;
    memset(&wc, 0, sizeof(WNDCLASSW));
    wc.lpfnWndProc = windowMessageProcess;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hi;
    wc.lpszClassName = reinterpret_cast<const wchar_t *>(class_name.utf16());
    ::RegisterClassW(&wc);

    hwnd = ::CreateWindowW(wc.lpszClassName, // classname
                           wc.lpszClassName, // window name
                           0,  // style
                           0,  // x
                           0,  // y
                           0,  // width
                           0,  // height
                           0,  // parent
                           0,  // menu handle
                           hi, // application
                           0); // windows creation data.
    if (!hwnd) {
        qDebug()<<"createMessageWindow error"<<(int)GetLastError();
    } else {
        // 初始化 DEV_BROADCAST_DEVICEINTERFACE 数据结构
        DEV_BROADCAST_DEVICEINTERFACE_W filter_data;
        memset(&filter_data, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE_W));
        filter_data.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE_W);
        filter_data.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        for (auto &&uuid : uuids)
        {
            filter_data.dbcc_classguid = uuid;
            HDEVNOTIFY handle = ::RegisterDeviceNotificationW(hwnd, &filter_data, DEVICE_NOTIFY_WINDOW_HANDLE);
            if (handle) {
                devNotifys.insert(uuid, handle);
            } else {
                qDebug()<<"RegisterDeviceNotification error"<<uuid;
            }
        }
        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)this);
    }
    return !!hwnd;
}

// 释放
void DeviceHotplugPrivate::destroyMessageWindow()
{
    if (hwnd) {
        ::DestroyWindow(hwnd);
        hwnd = nullptr;

        for (HDEVNOTIFY handle : qAsConst(devNotifys))
        {
            ::UnregisterDeviceNotification(handle);
        }
        devNotifys.clear();
    }
    ::UnregisterClassW(reinterpret_cast<const wchar_t *>(windowClassName().utf16()), ::GetModuleHandleW(nullptr));
}

DeviceHotplug::DeviceHotplug(QObject *parent)
    : QObject{parent}
    , dptr{new DeviceHotplugPrivate}
{
    dptr->ptr = this;
}

DeviceHotplug::~DeviceHotplug()
{
    free();
}

void DeviceHotplug::init(const QVector<QUuid> &uuids)
{
    const bool ret = dptr->createMessageWindow( uuids);
    if (!ret) {
        dptr->destroyMessageWindow();
    }
}

void DeviceHotplug::free()
{
    dptr->destroyMessageWindow();
}
