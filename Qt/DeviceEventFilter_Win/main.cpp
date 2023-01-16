#include <QApplication>
#include <QMainWindow>
#include "DeviceEventFilter.h"

#include <Windows.h>
#include <Dbt.h>
#include <devguid.h>
//具体的设备guid如usbiodef需要initguid
#include <initguid.h>
//USB设备
//GUID_DEVINTERFACE_USB_DEVICE
#include <usbiodef.h>
//HID人机交互设备-鼠标键盘等
#include <hidclass.h>
//GUID_DEVINTERFACE_KEYBOARD
#include <ntddkbd.h>
//GUID_DEVINTERFACE_MOUSE
#include <ntddmou.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow w;
    w.resize(600, 500);
    w.setWindowTitle("GongJianBo 1992");
    w.show();

    DeviceEventFilter filter;
    QVector<QUuid> uuids;
    //监测USB插拔
    uuids << GUID_DEVINTERFACE_USB_DEVICE;
    //监测鼠标键盘插拔，有些有扩展功能的鼠标会同时触发键盘和鼠标的插拔事件
    //uuids << GUID_DEVINTERFACE_KEYBOARD << GUID_DEVINTERFACE_MOUSE;
    filter.installFilter((HANDLE)w.winId(), uuids);
    app.installNativeEventFilter(&filter);

    return app.exec();
}
