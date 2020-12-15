#ifndef AUDIORECORDEROUTPUT_H
#define AUDIORECORDEROUTPUT_H

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

#include "AudioRecorderDevice.h"

/**
 * @brief 音频录制的输出
 * @author 龚建波
 * @date 2020-12-5
 * @note
 * 1.model要用QStringList而不是QList<QString>
 * 2.插拔设备后，deviceInfo不相等了
 */
class AudioRecorderOutput : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cacheDir READ getCacheDir WRITE setCacheDir NOTIFY cacheDirChanged)
    Q_PROPERTY(QStringList filterOutputDevicesName READ getFilterOutputDevicesName NOTIFY filterOutputDevicesNameChanged)
    Q_PROPERTY(QString outputDeviceName READ getOutputDeviceName WRITE setOutputDeviceName NOTIFY outputDeviceNameChanged)
public:
    explicit AudioRecorderOutput(QObject *parent = nullptr);
    ~AudioRecorderOutput();

    //缓存目录，配合不带路径的save函数
    QString getCacheDir() const { return cacheDir; }
    void setCacheDir(const QString &dir);

    //重新检测输出设备列表
    Q_INVOKABLE void updateOutputDevices();
    //检测当前使用的输出设备是否还存在，stop时别用
    Q_INVOKABLE bool checkOutputExists() const;
    //置为当前默认设备
    Q_INVOKABLE void resetToDefaultDevice();
    //可用的输出设备列表
    Q_INVOKABLE QStringList getAllOutputDevicesName() const;
    //过滤采样率后-可用的输出设备列表
    void setFilterOutputDevicesName(const QStringList &names);
    Q_INVOKABLE QStringList getFilterOutputDevicesName() const;

    //当前使用的输出设备
    Q_INVOKABLE QString getOutputDeviceName() const;
    Q_INVOKABLE void setOutputDeviceName(const QString &device);
    Q_INVOKABLE void setOutputDeviceIndex(int index);

private:
    //开始播放
    //io:回调处理数据
    //format:音频参数QAudioFormat，内部获取需要的成员值
    //device:输出设备名称，无则使用当前值
    bool startPlay(AudioRecorderDevice *io,const QAudioFormat &format,const QString &device=QString());
    //结束播放
    void stopPlay();
    //暂停播放
    void suspendPlay();
    //暂停后恢复
    void resumePlay();
    //释放
    void freePlay();

    //保存到文件
    bool saveToFile(const QByteArray data,const QAudioFormat &format,const QString &filepath);
    //保存到cache路径
    //（因为导入到音频库是以uuid为文件名，所以传入的文件名为uuid）
    //return 完整路径，路径为空则保存失败
    QString saveToCache(const QByteArray data,const QAudioFormat &format,const QString &uuid);

signals:
    void cacheDirChanged(const QString &dir);
    void stateChanged(QAudio::State state);
    void notify();
    void filterOutputDevicesNameChanged();
    void outputDeviceNameChanged();

private:
    //临时数据缓存目录
    QString cacheDir;
    //可用的输出设备列表
    QList<QAudioDeviceInfo> allOutputDevices;
    //过滤采样率后-可用的输出设备列表
    QList<QAudioDeviceInfo> filterOutputDevices;
    QList<QString> filterOutputNames;
    //当前使用的输出设备
    QAudioDeviceInfo outputDevice;
    //输出
    QAudioOutput *audioOutput=nullptr;
    //audioOutput当前使用的deviceInfo，start时不相同才重新new
    QAudioDeviceInfo audioDevice;
    //输出参数
    QAudioFormat outputFormat;

    friend class AudioRecorderView;
};

#endif // AUDIORECORDEROUTPUT_H
