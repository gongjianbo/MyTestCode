#pragma once
#include <QObject>
#include <objc/objc.h>

/**
 * @brief mac摄像头热插拔检测
 * @author 龚建波
 * @date 2023-03-22
 */
class AVDeviceHotplug : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AVDeviceHotplug)
    explicit AVDeviceHotplug(QObject *parent = nullptr);
public:
    ~AVDeviceHotplug();
    static AVDeviceHotplug *getInstance();

    // 注册热插拔回调
    void init();
    // 注销，会在析构中调用
    void free();

signals:
    // 设备插入
    void deviceAttached();
    // 设备拔出
    void deviceDetached();

private:
    id attachHandle = nullptr;
    id detachHandle = nullptr;
};
