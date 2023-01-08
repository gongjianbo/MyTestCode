#include "DSCameraCapture.h"
#include <QDebug>

//strmiids:DirectShow导出类标识符（CLSID）和接口标识符（IID）
#pragma comment(lib, "strmiids.lib")
//strmbase:DirectShow基类
#pragma comment(lib, "strmbase.lib")
//ole32:CoCreateInstance.CoInitialize
#pragma comment(lib, "ole32.lib")
//oleaut32:SysStringLen.VariantInit.VariantClear
#pragma comment(lib, "oleaut32.lib")

DSCameraCapture::DSCameraCapture()
{
    ::CoInitialize(NULL);
    //...
}

DSCameraCapture::~DSCameraCapture()
{
    freeHandle();
    //...
    ::CoUninitialize();
}

void DSCameraCapture::setCallback(const std::function<void (const QImage &)> &grabberCB,
                                  const std::function<void (const QImage &)> &stillCB)
{
    mGrabberCallback.callback = grabberCB;
    mStillCallback.callback = stillCB;
}

bool DSCameraCapture::enumDevices()
{
    //https://learn.microsoft.com/zh-cn/windows/win32/directshow/selecting-a-capture-device
    deviceList.clear();
    HRESULT hr = S_FALSE;

    //1.调用 CoCreateInstance 以创建系统设备枚举器的实例。
    ICreateDevEnum *pDevEnum = NULL;
    hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                            IID_ICreateDevEnum, reinterpret_cast<void **>(&pDevEnum));
    if (FAILED(hr) || !pDevEnum) {
        return false;
    }

    //2.调用 ICreateDevEnum::CreateClassEnumerator，并将设备类别指定为 GUID。
    IEnumMoniker *pEnumMoniker = NULL;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
    if (FAILED(hr) || !pEnumMoniker) {
        SAFE_RELEASE(pDevEnum);
        return false;
    }
    pEnumMoniker->Reset();

    //创建一个builder等下用来枚举格式信息
    ICaptureGraphBuilder2 *pGraphBuilder = NULL;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
                          IID_ICaptureGraphBuilder2, reinterpret_cast<void **>(&pGraphBuilder));
    if (FAILED(hr) || !pGraphBuilder){
        SAFE_RELEASE(pDevEnum);
        SAFE_RELEASE(pEnumMoniker);
        return false;
    }

    //3.CreateClassEnumerator 方法返回指向 IEnumMoniker 接口的指针。
    //若要枚举名字对象，请调用 IEnumMoniker::Next。
    IMoniker *pMoniker = NULL;
    IMalloc *mallocInterface = NULL;
    ::CoGetMalloc(1, (LPMALLOC*)&mallocInterface);
    while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK && pMoniker)
    {
        DSCameraDevice device;
        BSTR strName = NULL;
        hr = pMoniker->GetDisplayName(NULL, NULL, &strName);
        if (FAILED(hr)) {
            SAFE_RELEASE(pMoniker);
            continue;
        }
        //如"@device:pnp:\\\\?\\usb#vid_04ca&pid_7070&mi_00#6&16c57194&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global"
        device.displayName = QString::fromWCharArray(strName);
        mallocInterface->Free(strName);
        qDebug()<<"displayname"<<device.displayName;

        IPropertyBag *pPropBag = NULL;
        hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag,
                                     reinterpret_cast<void**>(&pPropBag));
        if (FAILED(hr) || !pPropBag) {
            SAFE_RELEASE(pMoniker);
            continue;
        }

        VARIANT var;
        var.vt = VT_BSTR;
        hr = pPropBag->Read(L"FriendlyName", &var, NULL);
        if (FAILED(hr)) {
            SAFE_RELEASE(pPropBag);
            SAFE_RELEASE(pMoniker);
            continue;
        }
        //如"Integrated Camera"
        device.friendlyName = QString::fromWCharArray(var.bstrVal);
        qDebug()<<"friendlyname"<<device.friendlyName;

        //displayname字符串里已经包含了devicepath信息
        //hr = pPropBag->Read(L"DevicePath", &var, NULL);
        //if (FAILED(hr)) {
        //    SAFE_RELEASE(pPropBag);
        //    SAFE_RELEASE(pMoniker);
        //    continue;
        //}
        //QString devicepath = QString::fromWCharArray(var.bstrVal);
        //qDebug()<<"devicepath"<<devicepath;
        //枚举该设备支持的格式和分辨率
        IBaseFilter *pSourceFilter = NULL;
        hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter,
                                    reinterpret_cast<void**>(&pSourceFilter));
        if (FAILED(hr) || !pSourceFilter) {
            SAFE_RELEASE(pPropBag);
            SAFE_RELEASE(pMoniker);
            continue;
        }
        device.resolutions = enumResolutions(pGraphBuilder, pSourceFilter);
        qDebug()<<"resolutions"<<device.resolutions;
        deviceList.push_back(device);

        SAFE_RELEASE(pSourceFilter);
        SAFE_RELEASE(pPropBag);
        SAFE_RELEASE(pMoniker);
    }

    SAFE_RELEASE(mallocInterface);
    SAFE_RELEASE(pGraphBuilder);
    SAFE_RELEASE(pMoniker);
    SAFE_RELEASE(pEnumMoniker);
    SAFE_RELEASE(pDevEnum);
    return true;
}

int DSCameraCapture::getDeviceCount() const
{
    return deviceList.count();
}

QStringList DSCameraCapture::getDeviceNames() const
{
    QStringList names;
    for (const auto &device : qAsConst(deviceList))
        names << device.displayName;
    return names;
}

QStringList DSCameraCapture::getFriendlyNames() const
{
    QStringList names;
    for (const auto &device : qAsConst(deviceList))
        names << device.friendlyName;
    return names;
}

DSCameraDevice DSCameraCapture::getDevice(const int index) const
{
    if (index < 0 || index >= getDeviceCount())
        return DSCameraDevice();
    return deviceList.at(index);
}

DSCameraDevice DSCameraCapture::getCurrentDevice() const
{
    return currentDevice;
}

QSize DSCameraCapture::getCurrentResolution() const
{
    return currentResolution;
}

bool DSCameraCapture::setCurrentResolution(const QSize &size)
{
    if (!currentDevice.isValid() || !mSourceFilter || !mBuilder)
        return false;
    QSize sizeSetting = size;
    if (!sizeSetting.isValid()) {
        if (currentDevice.resolutions.isEmpty()) {
            sizeSetting = QSize(1280, 720);
        } else {
            sizeSetting = currentDevice.resolutions.first();
        }
    }
    bool ret = false;
    HRESULT hr = S_FALSE;
    //设置分辨率，如果无效的话似乎会使用默认值
    IAMStreamConfig *pConfig = NULL;
    hr= mBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, mSourceFilter,
                                IID_IAMStreamConfig, reinterpret_cast<void **>(&pConfig));
    if (SUCCEEDED(hr)) {
        AM_MEDIA_TYPE *pmt = NULL;
        hr = pConfig->GetFormat(&pmt);
        if (SUCCEEDED(hr)) {
            VIDEOINFOHEADER *pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
            QSize sizeTemp = QSize(pvi->bmiHeader.biWidth, pvi->bmiHeader.biHeight);
            pvi->bmiHeader.biWidth = sizeSetting.width();
            pvi->bmiHeader.biHeight = sizeSetting.height();
            int fps = qRound(10000000.0 / pvi->AvgTimePerFrame);
            pvi->AvgTimePerFrame = 333333;
            hr = pConfig->SetFormat(pmt);
            if (SUCCEEDED(hr)) {
                currentResolution = sizeSetting;
                ret = true;
            } else {
                currentResolution = sizeTemp;
            }
            mGrabberCallback.width = currentResolution.width();
            mGrabberCallback.height = currentResolution.height();
            //mStillCallback.width = currentResolution.width();
            //mStillCallback.height = currentResolution.height();
            qDebug()<<__FUNCTION__<<hr<<"-size"<<currentResolution<<"-fps"<<fps<<"-per"<<pvi->AvgTimePerFrame;
            DSCameraUtil::DeleteMediaType(pmt);
        }
        SAFE_RELEASE(pConfig);
    }
    return ret;
}

bool DSCameraCapture::activeDevice(const int index, const QSize &size)
{
    auto &&device = getDevice(index);
    if (!device.isValid())
        return false;
    freeHandle();
    currentDevice = device;

    HRESULT hr = S_FALSE;
    //创建Filter Graph Manager.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                          IID_IGraphBuilder, reinterpret_cast<void **>(&mGraph));
    if (FAILED(hr))
        return false;
    //创建Capture Graph Builder.
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
                          IID_ICaptureGraphBuilder2, reinterpret_cast<void **>(&mBuilder));
    if (FAILED(hr))
        return false;
    //IMediaControl接口，用来控制流媒体在Filter Graph中的流动，例如流媒体的启动和停止；
    hr = mGraph->QueryInterface(IID_IMediaControl, reinterpret_cast<void **>(&mMediaControl));
    if (FAILED(hr))
        return false;

    mBuilder->SetFiltergraph(mGraph);
    //[note 这里移除了IID_IVideoWindow和IID_IMediaEvent]

    //创建用于预览的Sample Grabber Filter.
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL,CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void **>(&mSampleGrabberFilter));
    if(FAILED(hr))
        return false;
    //获取ISampleGrabber接口，用于设置回调等相关信息
    hr = mSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, reinterpret_cast<void **>(&mSampleGrabber));
    if(FAILED(hr))
        return false;
    //创建Null Filter
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void **>(&mNullFilter));
    if(FAILED(hr))
        return false;
    //创建用于抓拍的Sample Grabber Filter.
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL,CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void **>(&mSampleStillFilter));
    if(FAILED(hr))
        return false;
    //获取ISampleGrabber接口，用于设置回调等相关信息
    hr = mSampleStillFilter->QueryInterface(IID_ISampleGrabber, reinterpret_cast<void **>(&mSampleStill));
    if(FAILED(hr))
        return false;
    //创建Null Filter
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void **>(&mNullFilter2));
    if(FAILED(hr))
        return false;

    //[note 这里移除了CLSID_EZrgb24]
    if (!bindFilter())
        return false;

    hr = mGraph->AddFilter(mSourceFilter, L"Source Filter");
    if(FAILED(hr))
        return false;
    hr = mGraph->AddFilter(mSampleGrabberFilter, L"Sample Grabber Filter");
    if(FAILED(hr))
        return false;
    hr = mGraph->AddFilter(mSampleStillFilter, L"Sample Still Filter");
    if(FAILED(hr))
        return false;
    hr = mGraph->AddFilter(mNullFilter, L"Null Filter");
    if(FAILED(hr))
        return false;
    hr = mGraph->AddFilter(mNullFilter2, L"Null Filter2");
    if(FAILED(hr))
        return false;

    AM_MEDIA_TYPE mt = {0};
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    mt.formattype = FORMAT_VideoInfo;
    hr = mSampleGrabber->SetMediaType(&mt);
    if(FAILED(hr))
        return false;
    //回调函数 0-调用SampleCB 1-BufferCB
    mSampleGrabber->SetCallback(&mGrabberCallback, 1);
    if(FAILED(hr))
        return false;

    hr = mSampleStill->SetMediaType(&mt);
    if(FAILED(hr))
        return false;
    mSampleStill->SetOneShot(false);
    mSampleStill->SetBufferSamples(true);
    //回调函数 0-调用SampleCB 1-BufferCB
    mSampleStill->SetCallback(&mStillCallback, 1);
    if(FAILED(hr))
        return false;

    //设置分辨率
    setCurrentResolution(size);

    //RenderStream最后一个参数为空会弹出activemovie窗口显示预览视频
    hr = mBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                mSourceFilter, mSampleGrabberFilter, mNullFilter);
    if(FAILED(hr))
        return false;

    hr = mBuilder->RenderStream(&PIN_CATEGORY_STILL, &MEDIATYPE_Video,
                                mSourceFilter, mSampleStillFilter, mNullFilter2);
    if(FAILED(hr))
        return false;

    //LONGLONG start = 0, stop = MAXLONGLONG;
    //hr = mBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, mSourceFilter, &start, &stop, 1, 2);
    //if(FAILED(hr))
    //    return false;

    hr = mMediaControl->Run();
    if(FAILED(hr))
        return false;

    return true;
}

void DSCameraCapture::freeHandle()
{
    if (mMediaControl) {
        mMediaControl->Stop();
    }
    SAFE_RELEASE(mMediaControl);
    if (mGraph) {
        if (mSourceFilter) {
            freePin(mGraph, mSourceFilter);
            mGraph->RemoveFilter(mSourceFilter);
        }
        if (mSampleStillFilter) {
            freePin(mGraph, mSampleStillFilter);
            mGraph->RemoveFilter(mSampleStillFilter);
        }
        if (mSampleGrabberFilter) {
            freePin(mGraph, mSampleGrabberFilter);
            mGraph->RemoveFilter(mSampleGrabberFilter);
        }
        if (mNullFilter) {
            freePin(mGraph, mNullFilter);
            mGraph->RemoveFilter(mNullFilter);
        }
        if (mNullFilter2) {
            freePin(mGraph, mNullFilter2);
            mGraph->RemoveFilter(mNullFilter2);
        }
    }
    SAFE_RELEASE(mSourceFilter);
    SAFE_RELEASE(mNullFilter);
    SAFE_RELEASE(mNullFilter2);
    SAFE_RELEASE(mSampleStill);
    SAFE_RELEASE(mSampleStillFilter);
    SAFE_RELEASE(mSampleGrabber);
    SAFE_RELEASE(mSampleGrabberFilter);
    SAFE_RELEASE(mBuilder);
    SAFE_RELEASE(mGraph);
}

bool DSCameraCapture::bindFilter()
{
    if (!currentDevice.isValid())
        return false;

    HRESULT hr = S_FALSE;

    //1.调用 CoCreateInstance 以创建系统设备枚举器的实例。
    ICreateDevEnum *pDevEnum = NULL;
    hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                            IID_ICreateDevEnum, reinterpret_cast<void **>(&pDevEnum));
    if (FAILED(hr)) {
        return false;
    }

    //2.调用 ICreateDevEnum::CreateClassEnumerator，并将设备类别指定为 GUID。
    IEnumMoniker *pEnumMoniker = NULL;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
    if (FAILED(hr)) {
        SAFE_RELEASE(pDevEnum);
        return false;
    }
    pEnumMoniker->Reset();

    //3.CreateClassEnumerator 方法返回指向 IEnumMoniker 接口的指针。
    //若要枚举名字对象，请调用 IEnumMoniker::Next。
    IMoniker *pMoniker = NULL;
    IMalloc *mallocInterface = NULL;
    ::CoGetMalloc(1, (LPMALLOC*)&mallocInterface);
    while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK)
    {
        BSTR strName = NULL;
        hr = pMoniker->GetDisplayName(NULL, NULL, &strName);
        if (FAILED(hr)) {
            SAFE_RELEASE(pMoniker);
            continue;
        }
        //"@device:pnp:\\\\?\\usb#vid_04ca&pid_7070&mi_00#6&16c57194&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global"
        QString displayName = QString::fromWCharArray(strName);
        mallocInterface->Free(strName);
        //qDebug()<<"displayname"<<displayName;
        //匹配到选择的设备，参考Qt5.15DSCameraSession源码用的contains，我也不知道为啥不比较相等
        if (currentDevice.displayName.contains(displayName)) {
            //BindToObject将某个设备标识绑定到一个DirectShow Filter，
            //然后调用IFilterGraph::AddFilter加入到Filter Graph中，这个设备就可以参与工作了
            //调用IMoniker::BindToObject建立一个和选择的device联合的filter，
            //并且装载filter的属性(CLSID,FriendlyName, and DevicePath)。
            hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter,
                                        reinterpret_cast<void**>(&mSourceFilter));
            if (SUCCEEDED(hr)) {
                SAFE_RELEASE(pMoniker);
                break;
            }
        }
        SAFE_RELEASE(pMoniker);
    }

    SAFE_RELEASE(mallocInterface);
    SAFE_RELEASE(pMoniker);
    SAFE_RELEASE(pEnumMoniker);
    SAFE_RELEASE(pDevEnum);
    return !!mSourceFilter;
}

bool DSCameraCapture::checkMediaType(AM_MEDIA_TYPE *type) const
{
    if (type->majortype != MEDIATYPE_Video || type->formattype != FORMAT_VideoInfo)
    {
        return false;
    }

    VIDEOINFOHEADER *pvi = reinterpret_cast<VIDEOINFOHEADER *>(type->pbFormat);
    if (pvi->bmiHeader.biWidth <= 0 || pvi->bmiHeader.biHeight <= 0)
    {
        return false;
    }

    //数据格式判断，略
    //type->subtype
    return true;
}

QList<QSize> DSCameraCapture::enumResolutions(ICaptureGraphBuilder2 *pGraphBuilder,
                                              IBaseFilter *pSourceFilter) const
{
    QList<QSize> resolutions;
    if (!pGraphBuilder || !pSourceFilter)
        return resolutions;

    HRESULT hr = S_FALSE;

    VIDEO_STREAM_CONFIG_CAPS scc;
    IAMStreamConfig *pConfig = NULL;

    hr = pGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                      pSourceFilter, IID_IAMStreamConfig,
                                      reinterpret_cast<void**>(&pConfig));
    if (FAILED(hr)) {
        return resolutions;
    }

    int iCount = 0;
    int iSize = 0;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (FAILED(hr)) {
        SAFE_RELEASE(pConfig);
        return resolutions;
    }

    for (int iIndex = 0; iIndex < iCount; ++iIndex) {
        //获取支持的分辨率、格式等信息
        AM_MEDIA_TYPE *pmt = NULL;
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (FAILED(hr)) {
            continue;
        }
        if (!checkMediaType(pmt)) {
            DSCameraUtil::DeleteMediaType(pmt);
            continue;
        }

        VIDEOINFOHEADER *pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
        //qDebug()<<iIndex<<DSCameraUtil::SubtypeToText(pmt->subtype)
        //<<pvi->bmiHeader.biWidth<<pvi->bmiHeader.biHeight<<pvi->AvgTimePerFrame;
        QSize size = QSize(pvi->bmiHeader.biWidth, pvi->bmiHeader.biHeight);
        //格式目前取默认，遍历出来size不是有序的
        if (!resolutions.contains(size))
            resolutions.push_back(size);

        DSCameraUtil::DeleteMediaType(pmt);
    }

    SAFE_RELEASE(pConfig);
    return resolutions;
}

void DSCameraCapture::freePin(IGraphBuilder *inGraph, IBaseFilter *inFilter) const
{
    if (!inGraph || !inFilter)
        return;

    IEnumPins *pinEnum = NULL;

    // 创建一个Pin的枚举器
    if (SUCCEEDED(inFilter->EnumPins(&pinEnum)))
    {
        pinEnum->Reset();

        IPin *pin = NULL;
        ULONG fetched = 0;
        // 枚举该Filter上所有的Pin
        while (SUCCEEDED(pinEnum->Next(1, &pin, &fetched)) && fetched)
        {
            if (pin)
            {
                // 得到当前Pin连接对象的Pin指针
                IPin * connectedPin = NULL;
                pin->ConnectedTo(&connectedPin);
                if (connectedPin)
                {
                    // 查询Pin信息（获取Pin的方向)
                    PIN_INFO pininfo;
                    if (SUCCEEDED(connectedPin->QueryPinInfo(&pininfo)))
                    {
                        pininfo.pFilter->Release();
                        if (pininfo.dir == PINDIR_INPUT)
                        {
                            // 如果连接对方是输入Pin(说明当前枚举得到的是输出Pin)
                            // 则递归调用NukeDownstream函数，首先将下一级（乃至再下一级）
                            // 的所有Filter删除
                            freePin(inGraph, pininfo.pFilter);
                            inGraph->Disconnect(connectedPin);
                            inGraph->Disconnect(pin);
                            inGraph->RemoveFilter(pininfo.pFilter);
                        }
                    }
                    connectedPin->Release();
                }
                pin->Release();
            }
        }
        pinEnum->Release();
    }
}
