#ifndef AUDIORECORDERINPUT_H
#define AUDIORECORDERINPUT_H

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioInput>

#include "AudioRecorderDevice.h"

/**
 * @brief 音频录制的输入
 * @author 龚建波
 * @date 2020-12-5
 * @note
 * model要用QStringList而不是QList<QString>
 *
 * @note
 * 1.获取输入设备名重复是插件问题，在新版本可以判断插件（如5.15）
 * 参见：（Qt4）QTBUG-16841（Qt5）QTBUG-75781
 * 暂时用采样率来过滤输入设备
 * 2.插拔设备后，deviceInfo不相等了
 */
class AudioRecorderInput : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList filterInputDevicesName READ getFilterInputDevicesName NOTIFY filterInputDevicesNameChanged)
    Q_PROPERTY(QString inputDeviceName READ getInputDeviceName WRITE setInputDeviceName NOTIFY inputDeviceNameChanged)
public:
    explicit AudioRecorderInput(QObject *parent = nullptr);
    ~AudioRecorderInput();

    //重新检测输入设备列表
    Q_INVOKABLE void updateInputDevices();
    //检测当前使用的输入设备是否还存在，stop时别用
    Q_INVOKABLE bool checkInputExists() const;
    //置为当前默认设备
    Q_INVOKABLE void resetToDefaultDevice();
    //可用的输入设备列表
    Q_INVOKABLE QStringList getAllInputDevicesName() const;
    //过滤采样率后-可用的输入设备列表
    Q_INVOKABLE QStringList getFilterInputDevicesName() const;

    //当前使用的输入设备
    Q_INVOKABLE QString getInputDeviceName() const;
    Q_INVOKABLE void setInputDeviceName(const QString &device);
    Q_INVOKABLE void setInputDeviceIndex(int index);
    //判断该设备是否可用
    Q_INVOKABLE bool isAvailableDevice(const QString &device);

private:
    //开始录音
    //io:回调处理数据
    //format:音频参数QAudioFormat，内部获取需要的成员值
    //device:输入设备名称，无则使用当前值
    bool startRecord(AudioRecorderDevice *io,const QAudioFormat &format,const QString &device=QString());
    //结束录制
    void stopRecord();
    //结束并释放
    void freeRecord();
    //从文件读取
    //暂时不带解析器，只能解析44字节固定wav-pcm
    //（即本组件生成的wav文件）
    bool loadFromFile(AudioRecorderDevice *io,const QString &filepath);

signals:
    void stateChanged(QAudio::State state);
    void notify();
    void filterInputDevicesNameChanged();
    void inputDeviceNameChanged();

private:
    //可用的输入设备列表
    QList<QAudioDeviceInfo> allInputDevices;
    //过滤采样率后-可用的输入设备列表
    QList<QAudioDeviceInfo> filterInputDevices;
    //当前使用的输入设备
    QAudioDeviceInfo inputDevice;
    //获取数据
    QAudioInput *audioInput=nullptr;
    //audioInput当前使用的deviceInfo，start时不相同才重新new
    QAudioDeviceInfo audioDevice;
    //输入参数
    QAudioFormat inputFormat;

    friend class AudioRecorderView;
};

#endif // AUDIORECORDERINPUT_H
