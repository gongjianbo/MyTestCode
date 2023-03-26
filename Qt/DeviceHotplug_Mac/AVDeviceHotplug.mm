#include "AVDeviceHotplug.h"
#include <AVFoundation/AVFoundation.h>
//#include <QCameraInfo>
//#include <QDebug>

AVDeviceHotplug::AVDeviceHotplug(QObject *parent)
    : QObject(parent)
{

}

AVDeviceHotplug::~AVDeviceHotplug()
{
    free();
}

AVDeviceHotplug *AVDeviceHotplug::getInstance()
{
    static AVDeviceHotplug instance;
    return &instance;
}

void AVDeviceHotplug::init()
{
    // 注册之前必须先调用AVCaptureDevice
    [AVCaptureDevice devices];
    // 摄像头可用事件
    attachHandle = [[NSNotificationCenter defaultCenter]
            addObserverForName:AVCaptureDeviceWasConnectedNotification
                               object:nil
                               queue:nil
                               usingBlock:^(NSNotification *){
        //qDebug()<<QCameraInfo::availableCameras().size();
        emit deviceAttached();
    }];
    // 摄像头不可用
    detachHandle = [[NSNotificationCenter defaultCenter]
            addObserverForName:AVCaptureDeviceWasDisconnectedNotification
                               object:nil
                               queue:nil
                               usingBlock:^(NSNotification *){
        //qDebug()<<QCameraInfo::availableCameras().size();
        emit deviceDetached();
    }];
}

void AVDeviceHotplug::free()
{
    if (attachHandle) {
        [[NSNotificationCenter defaultCenter] removeObserver:attachHandle];
    }
    if (detachHandle) {
        [[NSNotificationCenter defaultCenter] removeObserver:detachHandle];
    }
}
