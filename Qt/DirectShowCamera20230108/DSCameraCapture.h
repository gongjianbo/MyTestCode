#pragma once
#include <QObject>
#include "DSCameraCommon.h"

/**
 * @brief DirectShow相机相关接口操作
 * @author 龚建波
 * @date 2022-12-14
 * @details
 * 1.AM_MEDIA_TYPE 结构体表示媒体信息
 *   subtype字段表示格式信息，如RGB24\YUV2等
 *   pbFormat转为VIDEOINFOHEADER可以获取分辨率宽高信息，biWidth\biHeight
 * 2.动态设置分辨率待解决，暂时先全部释放再初始化
 * 3.still捕获的图像分辨率待解决，只设置上了预览的分辨率，暂时用预览的图
 * 4.帧率暂时固定为30fps，但是实测没达到30
 */
class DSCameraCapture
{
public:
    DSCameraCapture();
    ~DSCameraCapture();

    //设置回调
    void setCallback(const std::function<void(const QImage &image)> &grabberCB,
                     const std::function<void(const QImage &image)> &stillCB);
    //枚举设备信息
    bool enumDevices();
    //枚举到的设备个数
    int getDeviceCount() const;
    //设备名列表-displayName
    QStringList getDeviceNames() const;
    //设备名对应的显示名称-friendlyName
    QStringList getFriendlyNames() const;

    //根据deviceList.index获取一个设备信息DSCameraDevice，需判断返回结果是否有效
    DSCameraDevice getDevice(const int index) const;
    //当前使用的设备信息
    DSCameraDevice getCurrentDevice() const;
    //当前使用的分辨率
    QSize getCurrentResolution() const;
    bool setCurrentResolution(const QSize &size);
    //选择对应的设备和参数初始化
    bool activeDevice(const int index, const QSize &size = QSize());
    //停止并释放，每次初始化前需要调用
    void freeHandle();

private:
    //initDevice中调用，先提前设置currentDevice
    bool bindFilter();
    //判断AM_MEDIA_TYPE信息是否有效
    bool checkMediaType(AM_MEDIA_TYPE *type) const;
    //枚举分辨率信息
    QList<QSize> enumResolutions(ICaptureGraphBuilder2 *pGraphBuilder,
                                 IBaseFilter *pSourceFilter) const;
    //删除与该Filter连接的下游的所有Filter，从旧工程copy
    void freePin(IGraphBuilder *inGraph, IBaseFilter *inFilter) const;

private:
    //当前应用的设备信息
    DSCameraDevice currentDevice;
    //枚举出的设备列表
    QList<DSCameraDevice> deviceList;
    //当前选择的分辨率
    QSize currentResolution;

    ICaptureGraphBuilder2 *mBuilder{NULL};
    IGraphBuilder *mGraph{NULL};
    IMediaControl *mMediaControl{NULL};

    //预览
    ISampleGrabber *mSampleGrabber{NULL};
    IBaseFilter *mSampleGrabberFilter{NULL};
    //硬件拍图
    ISampleGrabber *mSampleStill{NULL};
    IBaseFilter *mSampleStillFilter{NULL};

    IBaseFilter *mNullFilter{NULL};
    IBaseFilter *mNullFilter2{NULL};
    IBaseFilter *mSourceFilter{NULL};

    DSGrabberCB mGrabberCallback;
    DSGrabberCB mStillCallback;
};
