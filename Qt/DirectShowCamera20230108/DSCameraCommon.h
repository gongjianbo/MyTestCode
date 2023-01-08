#pragma once
#include <memory>
#include <functional>
#include <QObject>
#include <QVector>
#include <QSet>
#include <QList>
#include <QSize>
#include <QImage>
#include "DSSampleGrabber.h"
//该文件放一些公共的定义

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr) { if (ptr) ptr->Release(); ptr = NULL; }
#endif

//AM_MEDIA_TYPE的subtype字段表示编码信息，如RGB YUV等
struct DSTypeInfo
{
    QString name;
    GUID typeGuid;
};

//编码GUID和字符串的映射表
static DSTypeInfo DsTypeTable[] = {
    {"ARGB32", MEDIASUBTYPE_ARGB32},
    {"RGB32", MEDIASUBTYPE_RGB32},
    {"RGB24", MEDIASUBTYPE_RGB24},
    {"RGB555", MEDIASUBTYPE_RGB555},
    {"RGB565", MEDIASUBTYPE_RGB565},
    {"MJPG", MEDIASUBTYPE_MJPG},
    {"JPG", MEDIASUBTYPE_IJPG},
    {"AYUV", MEDIASUBTYPE_AYUV},
    {"IYUV", MEDIASUBTYPE_IYUV},
    {"YV12", MEDIASUBTYPE_YV12},
    {"UYVY", MEDIASUBTYPE_UYVY},
    {"YUYV", MEDIASUBTYPE_YUYV},
    {"YUY2", MEDIASUBTYPE_YUY2},
    {"NV12", MEDIASUBTYPE_NV12},
    {"IMC1", MEDIASUBTYPE_IMC1},
    {"IMC2", MEDIASUBTYPE_IMC2},
    {"IMC3", MEDIASUBTYPE_IMC3},
    {"IMC4", MEDIASUBTYPE_IMC4}
};

//辅助接口
namespace DSCameraUtil
{
//释放AM_MEDIA_TYPE
inline void FreeMediaType(AM_MEDIA_TYPE &type) {
    if (type.cbFormat > 0)
        ::CoTaskMemFree(type.pbFormat);

    if (type.pUnk)
        type.pUnk->Release();

    SecureZeroMemory(&type, sizeof(type));
}

//释放AM_MEDIA_TYPE*
inline void DeleteMediaType(AM_MEDIA_TYPE *type) {
    if (!type)
        return;
    FreeMediaType(*type);
    ::CoTaskMemFree(type);
}

//QSize比较
inline bool SizeLess(const QSize &left, const QSize &right) {
    return left.width() < right.width() && left.height() < right.height();
}

//AM_MEDIA_TYPE的格式信息GUID转字符串
inline QString SubtypeToText(GUID subtype) {
    for (const auto &typeInfo : DsTypeTable) {
        if (IsEqualGUID(typeInfo.typeGuid, subtype))
            return typeInfo.name;
    }
    return "Invalid";
}
}

//设备信息
//目前只用到了设备名和分辨率
//支持的格式(rgb\yuv)暂时固定就不枚举出来了
struct DSCameraDevice
{
    //一串奇怪的字符串，包含devicePath等信息
    QString displayName;
    //用于ui显示的设备名
    QString friendlyName;
    //支持的分辨率
    QList<QSize> resolutions;

    //重置信息
    void clear() {
        displayName.clear();
        friendlyName.clear();
        resolutions.clear();
    }
    //是否有效，主要区分返回值默认构造
    bool isValid() const {
        return !displayName.isEmpty();
    }
};

/**
 * @brief 视频流的回调
 * @author 龚建波
 * @date 2022-12-14
 * @details
 * ISampleGrabber::SetCallback(ISampleGrabberCB * pCallback, long WhichMethodToCallback)
 * 参数1回调函数，参数2模式为0-调用SampleCB，为1-调用BufferCB
 */
class DSGrabberCB : public ISampleGrabberCB
{
public:
    ULONG STDMETHODCALLTYPE AddRef() override {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release() override {
        return 1;
    }
    HRESULT STDMETHODCALLTYPE SampleCB(double time, IMediaSample *sample) override {
        Q_UNUSED(time)
        Q_UNUSED(sample)
        return S_OK;
    }
    //
    HRESULT STDMETHODCALLTYPE BufferCB(double time, BYTE *buffer, long len) override {
        Q_UNUSED(time)
        if (buffer && len > 0 && len == width * height * 3) {
            QImage img(buffer, width, height, width * 3, QImage::Format_BGR888);
            //反正要copy一份，正好翻转一下
            img = img.mirrored(false, true);
            callback(img);
        } else {
            callback(QImage());
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) override {
        if(iid == IID_ISampleGrabberCB || iid == IID_IUnknown) {
            *ppv = reinterpret_cast<LPVOID*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    //提前设置好回调
    std::function<void(const QImage &image)> callback;
    int width{0};
    int height{0};
};
