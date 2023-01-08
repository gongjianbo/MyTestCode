#include "UVCCore.h"
#include "DSCameraCapture.h"
#include "HandyConfig.h"
#include "HandyTool.h"
#include "kohn_avi.h"
#include <thread>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QEventLoop>
#include <QProcess>
#include <QDebug>

UVCCore::UVCCore(QObject *parent)
    : QObject{parent}
{
    //后面根据对应的库来加载
    control.reset(new DSCameraCapture);

    //录制定时器
    recordTimer.setInterval(1000);
    connect(&recordTimer, &QTimer::timeout, this, [this](){
        setDuration(getDuration() + 1);
        //多录了一秒校准时间
        if (getDuration() > 15) {
            stopRecord();
        }
    });

    //通过回调函数取图片数据
    control->setCallback([this](const QImage &image){
        //qDebug()<<"grabber"<<QTime::currentTime()<<image.isNull()<<image.size();
        emit grabberImageComing(image);
        //判断是否需要抓图，still回调和cpature调用会设置falg
        if (captureFlag) {
            const QString path = HandyConfigPtr->getCachePath(TARGET_NAME, "capture.jpg");
            qDebug()<<"capture"<<path;
            if (image.save(path, "jpg")) {
                emit captureFinished(QUrl::fromLocalFile(path));
            }
            captureFlag = false;
        } else if (recordFlag && recordCounter >= 0) {
            std::lock_guard<std::mutex> guard(recordMutex); Q_UNUSED(guard);
            if (recordHandle) {
                QByteArray arr;
                QBuffer buffer(&arr);
                image.save(&buffer, "JPEG");
                kavi_add_frame(recordHandle, (const unsigned char*)arr.constData(), arr.size());
                if (recordCounter == 0) {
                    const QString path = HandyConfigPtr->getCachePath(TARGET_NAME, "record.jpg");
                    image.save(path, "jpg");
                    captureFlag = false;
                }
                recordCounter++;
            }
        }
    }, [this](const QImage &image){
        //qDebug()<<"still"<<QTime::currentTime()<<image.isNull()<<image.size();
        emit stillImageComing(image);
        //设置抓图标志，等待下一次grabberImageComing
        captureFlag = true;
    });
}

UVCCore::~UVCCore()
{
    freeRecord();
}

int UVCCore::getDeviceCount() const
{
    return control->getDeviceCount();
}

QStringList UVCCore::getDeviceNames() const
{
    return control->getDeviceNames();
}

QStringList UVCCore::getFriendlyNames() const
{
    return control->getFriendlyNames();
}

QStringList UVCCore::getResolutions() const
{
    QStringList resolutions;
    const auto &temp = control->getCurrentDevice().resolutions;
    for (const QSize &size : temp)
    {
        resolutions << sizeToStr(size);
    }
    return resolutions;
}

int UVCCore::getDeviceIndex() const
{
    return currentDeviceIndex;
}

void UVCCore::setDeviceIndex(const int index)
{
    deviceSetting(index, currentResolution);
}

QSize UVCCore::getDeviceResolution() const
{
    return currentResolution;
}

void UVCCore::setDeviceResolution(const QSize &size)
{
    deviceSetting(currentDeviceIndex, size);
    /*if (currentResolution != size) {
        const auto &temp = control->getCurrentDevice().resolutions;
        if (temp.contains(size)) {
            control->setCurrentResolution(size);
            const auto &&resolution = control->getCurrentResolution();
            if (currentResolution != resolution) {
                currentResolution = resolution;
                emit currentResolutionChanged();
            }
        }
    }*/
}

bool UVCCore::getOnRecord() const
{
    return recordFlag;
}

void UVCCore::setOnRecord(bool flag)
{
    if (recordFlag != flag) {
        recordFlag = flag;
        emit onRecordChanged();
    }
}

int UVCCore::getDuration() const
{
    return recordDuration;
}

void UVCCore::setDuration(int duration)
{
    if (recordDuration != duration) {
        recordDuration = duration;
        emit durationChanged();
    }
}

void UVCCore::enumDevices()
{
    //枚举前先暂存
    QString old_name;
    auto &&old_device = getDeviceNames();
    if (currentDeviceIndex >= 0 && currentDeviceIndex < old_device.size())
        old_name = old_device.at(currentDeviceIndex);
    control->enumDevices();
    emit deviceListChanged();

    //以设备名恢复之前的设置
    auto &&new_device = getDeviceNames();
    currentDeviceIndex = -1;
    setDeviceIndex(new_device.indexOf(old_name));
}

void UVCCore::deviceSetting(const int index, const QSize &size)
{
    int deviceIndex = index;
    if (deviceIndex < 0)
        deviceIndex = 0;
    if (deviceIndex >= getDeviceCount())
        deviceIndex = 0;
    if (currentDeviceIndex != deviceIndex || currentResolution != size) {
        control->activeDevice(deviceIndex, size);
        currentDeviceIndex = deviceIndex;
        emit currentDeviceChanged();
        const auto &&resolution = control->getCurrentResolution();
        if (currentResolution != resolution) {
            currentResolution = resolution;
            emit currentResolutionChanged();
        }
    }
}

QStringList UVCCore::deviceResolutions(const int index) const
{
    QStringList resolutions;
    const auto &temp = control->getDevice(index).resolutions;
    for (const QSize &size : temp)
    {
        resolutions << sizeToStr(size);
    }
    return resolutions;
}

void UVCCore::stop()
{
    control->freeHandle();
}

QSize UVCCore::strToSize(const QString &str)
{
    QSize size;
    QStringList split = str.split('*');
    if (split.size() == 2) {
        size = QSize(split.first().trimmed().toInt(),
                     split.last().trimmed().toInt());
    }
    return size;
}

QString UVCCore::sizeToStr(const QSize &size)
{
    return QString("%1 * %2").arg(size.width()).arg(size.height());
}

void UVCCore::printThread(const QString &tip)
{
    qDebug()<<__FUNCTION__<<QThread::currentThread()<<tip;
}

void UVCCore::capture()
{
    captureFlag = true;
}

void UVCCore::startRecord()
{
    if (getOnRecord())
        stopRecord();
    const auto &&device = control->getCurrentDevice();
    if (!device.isValid())
        return;
    const QString path = HandyConfigPtr->getCachePath(TARGET_NAME, "record.avi");
    qDebug()<<"record"<<path;
    QSize size = control->getCurrentResolution();
    freeRecord();
    if (QFile::exists(path)) {
        QFile::remove(path);
    }
    {
        std::lock_guard<std::mutex> guard(recordMutex); Q_UNUSED(guard);
        QByteArray path_arr = path.toLocal8Bit();
        recordHandle = kavi_open(path_arr.constData(), size.width(), size.height(),
                                 "MJPG", 30, NULL);
        if (recordHandle) {
            setDuration(0);
            recordCounter = 0;
            setOnRecord(true);
            recordTimer.start();
        }
    }
}

void UVCCore::stopRecord()
{
    recordTimer.stop();
    if (!getOnRecord())
        return;
    freeRecord();
    //setOnRecord(false);
    std::thread th([this](){
        std::lock_guard<std::mutex> guard(recordMutex); Q_UNUSED(guard);
        const QString app_dir = HandyConfigPtr->getAppPath();
        const QString avi_path = HandyConfigPtr->getCachePath(TARGET_NAME, "record.avi");
        const QString mp4_path = HandyConfigPtr->getCachePath(TARGET_NAME, "record.mp4");
        const QString image_path = HandyConfigPtr->getCachePath(TARGET_NAME, "record.jpg");
        if (QFile::exists(mp4_path)) {
            QFile::remove(mp4_path);
        }

        QEventLoop loop;
        QProcess process;
        connect(&process, &QProcess::readyReadStandardOutput, [&process](){
            qDebug()<<process.readAllStandardOutput();
        });
        connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [&](int exitCode, QProcess::ExitStatus exitStatus){
            exitCode; exitStatus;
            loop.quit();
        });
        process.setProcessChannelMode(QProcess::MergedChannels);

        QStringList args;
        args << "-y"
             << "-i" << avi_path
             << "-f" << "mp4"
             << "-qscale" << "5"
             << "-vcodec" << "mpeg4"
             << "-acodec" << "aac"
             << "-t" << "15"
             << mp4_path;
        process.start(app_dir + "/codecs/ffmpeg.exe", args);
        loop.exec();

        emit recordFinished(QUrl::fromLocalFile(mp4_path),
                            QUrl::fromLocalFile(image_path));
        QMetaObject::invokeMethod(this, [this](){
            setOnRecord(false);
        });
    });
    th.detach();
}

void UVCCore::freeRecord()
{
    std::lock_guard<std::mutex> guard(recordMutex); Q_UNUSED(guard);
    if (recordHandle) {
        float fps = 30.0f;
        if (recordCounter > 0) {
            fps = recordCounter / (float)getDuration();
            if (fps < 0) {
                fps = 30.0f;
            }
        }
        //校准fps
        kavi_set_framerate(recordHandle, fps);
        kavi_close(recordHandle);
        recordHandle = nullptr;
    }
    recordCounter = -1;
}
