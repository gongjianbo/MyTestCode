#pragma once
#include <atomic>
#include <mutex>
#include <QObject>
#include <QSharedPointer>
#include <QSize>
#include <QImage>
#include <QUrl>
#include <QTimer>
#include "HandyCore.h"

class DSCameraCapture;
struct kavi_t;

/**
 * @brief UVC相关功能调用
 * @author 龚建波
 * @date 2022-12-14
 * @details
 * 1.View负责显示图像，Core负责具体的操作
 * 2.目前只做了directshow实现，后面引入别的库增加抽象接口
 */
class UVCCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int deviceCount READ getDeviceCount NOTIFY deviceListChanged)
    Q_PROPERTY(QStringList deviceNames READ getDeviceNames NOTIFY deviceListChanged)
    Q_PROPERTY(QStringList friendlyNames READ getFriendlyNames NOTIFY deviceListChanged)
    Q_PROPERTY(QStringList resolutions READ getResolutions NOTIFY currentDeviceChanged)
    Q_PROPERTY(int deviceIndex READ getDeviceIndex WRITE setDeviceIndex NOTIFY currentDeviceChanged)
    Q_PROPERTY(QSize deviceResolution READ getDeviceResolution WRITE setDeviceResolution NOTIFY currentResolutionChanged)
    Q_PROPERTY(bool onRecord READ getOnRecord WRITE setOnRecord NOTIFY onRecordChanged)
    Q_PROPERTY(int duration READ getDuration WRITE setDuration NOTIFY durationChanged)
public:
    explicit UVCCore(QObject *parent = nullptr);
    ~UVCCore();

    //枚举到的设备个数
    int getDeviceCount() const;
    //设备名列表-displayName
    QStringList getDeviceNames() const;
    //设备名对应的显示名称-friendlyName
    QStringList getFriendlyNames() const;
    //当前设备支持的分辨率
    QStringList getResolutions() const;

    //根据devicenames顺序选择一个设备
    int getDeviceIndex() const;
    void setDeviceIndex(const int index);

    //当前的分辨率设置
    QSize getDeviceResolution() const;
    void setDeviceResolution(const QSize &size);

    //录制状态
    bool getOnRecord() const;
    void setOnRecord(bool flag);

    //录制时长
    int getDuration() const;
    void setDuration(int duration);

    //枚举设备信息
    Q_INVOKABLE void enumDevices();
    //设置设备的和分辨率
    Q_INVOKABLE void deviceSetting(const int index, const QSize &size);
    //设备支持的分辨率
    Q_INVOKABLE QStringList deviceResolutions(const int index) const;
    //停止预览
    Q_INVOKABLE void stop();
    //分辨率字符串转size
    Q_INVOKABLE static QSize strToSize(const QString &str);
    //分辨率size转字符串
    Q_INVOKABLE static QString sizeToStr(const QSize &size);
    //测试，打印当前线程
    Q_INVOKABLE static void printThread(const QString &tip);

    //捕获一张图
    Q_INVOKABLE void capture();
    //开始录制视频，15s
    Q_INVOKABLE void startRecord();
    //停止录制视频
    Q_INVOKABLE void stopRecord();
    //释放录制
    void freeRecord();

signals:
    //预览图像，信号来自回调线程
    void grabberImageComing(const QImage &image);
    //硬件拍图，信号来自回调线程
    void stillImageComing(const QImage &image);
    //枚举设备信息后发出信号
    void deviceListChanged();
    //当前设备切换
    void currentDeviceChanged();
    //分辨率切换
    void currentResolutionChanged();
    //录制状态
    void onRecordChanged();
    //录制时长s
    void durationChanged();

    //抓图完成后信号通知
    void captureFinished(const QUrl &imageUrl);
    //录制完成后信号通知
    void recordFinished(const QUrl &videoUrl, const QUrl &imageUrl);

private:
    QSharedPointer<DSCameraCapture> control;
    //当前使用的设备信息的下标，对应的数组放在control中
    int currentDeviceIndex{-1};
    //当前使用的分辨率
    QSize currentResolution;
    //拍图标志，在回调中判断
    std::atomic_bool captureFlag{false};

    //录制标志，在回调中判断
    std::atomic_bool recordFlag{false};
    //录制时长
    int recordDuration{0};
    //录制帧计数
    std::atomic_int recordCounter{-1};
    //handle加锁
    std::mutex recordMutex;
    //视频录制
    kavi_t *recordHandle{nullptr};
    //定时录制15s
    QTimer recordTimer;
};
