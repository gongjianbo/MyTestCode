#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include "DeviceHotplug.h"

#include <Windows.h>
#include <Dbt.h>
#include <devguid.h>
// 具体的设备 GUID 需要 initguid, 如 usbiodef
#include <initguid.h>
// USB 设备
// GUID_DEVINTERFACE_USB_DEVICE
#include <usbiodef.h>
// HID 人机交互设备-鼠标键盘等
#include <hidclass.h>
// 键盘 GUID_DEVINTERFACE_KEYBOARD
#include <ntddkbd.h>
// 鼠标 GUID_DEVINTERFACE_MOUSE
#include <ntddmou.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceHotplug filter;
    QVector<QUuid> uuids;
    // 监测 USB 插拔
    uuids << GUID_DEVINTERFACE_USB_DEVICE;
    // 监测鼠标键盘插拔，有些有扩展功能的鼠标会同时触发键盘和鼠标的插拔事件
    // uuids << GUID_DEVINTERFACE_KEYBOARD << GUID_DEVINTERFACE_MOUSE;
    filter.init(uuids);
    QObject::connect(&filter, &DeviceHotplug::deviceAttached,
                     &app, [](quint16 vid, quint16 pid){
        qDebug()<<"attached"<<QString::number(vid, 16)<<QString::number(pid, 16);
    });
    QObject::connect(&filter, &DeviceHotplug::deviceDetached,
                     &app, [](quint16 vid, quint16 pid){
        qDebug()<<"detached"<<QString::number(vid, 16)<<QString::number(pid, 16);
    });

    QMainWindow w;
    w.resize(600, 500);
    w.setWindowTitle("DeviceHotplug (by GongJianBo 1992)");
    w.show();

    return app.exec();
}
