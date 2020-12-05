#include "AudioRecorderOutput.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>

AudioRecorderOutput::AudioRecorderOutput(QObject *parent) : QObject(parent)
{
    //采样精度和声道数暂时默认
    outputFormat.setSampleRate(16000);
    outputFormat.setChannelCount(1);
    outputFormat.setSampleSize(16);
    outputFormat.setCodec("audio/pcm");
    outputFormat.setByteOrder(QAudioFormat::LittleEndian);
    outputFormat.setSampleType(QAudioFormat::SignedInt);

    checkOutputDevices();
    resetToDefaultDevice();
    //if(filterOutputDevices.count()>0)
    //    outputDevice=filterOutputDevices.first();
}

AudioRecorderOutput::~AudioRecorderOutput()
{
    stopPlay();
}

void AudioRecorderOutput::checkOutputDevices()
{
    allOutputDevices=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    filterOutputDevices.clear();

    //根据采样率过滤输入设备
    for(auto &info:allOutputDevices)
    {
        if(info.supportedSampleRates().contains(outputFormat.sampleRate()))
            filterOutputDevices.push_back(info);
    }

    emit filterOutputDevicesNameChanged();
}

void AudioRecorderOutput::resetToDefaultDevice()
{
    outputDevice=QAudioDeviceInfo::defaultOutputDevice();
}

QList<QString> AudioRecorderOutput::getAllOutputDevicesName() const
{
    QList<QString> name_list;
    for(auto &info:allOutputDevices)
        name_list.push_back(info.deviceName());
    return name_list;
}

QList<QString> AudioRecorderOutput::getFilterOutputDevicesName() const
{
    QList<QString> name_list;
    for(auto &info:filterOutputDevices)
        name_list.push_back(info.deviceName());
    return name_list;
}

QString AudioRecorderOutput::getOutputDeviceName() const
{
    return outputDevice.deviceName();
}

void AudioRecorderOutput::setOutputDeviceName(const QString &device)
{
    outputDevice=QAudioDeviceInfo();
    for(auto &info:filterOutputDevices)
    {
        if(info.deviceName()==device){
            outputDevice=info;
            break;
        }
    }
}

void AudioRecorderOutput::setOutputDeviceIndex(int index)
{
    if(index<0||index>=filterOutputDevices.count())
        return;
    outputDevice=filterOutputDevices.at(index);
}

bool AudioRecorderOutput::startPlay(AudioRecorderDevice *io, const QAudioFormat &format, const QString &device)
{
    stopPlay();

    if(format.sampleRate()>0)
        outputFormat.setSampleRate(format.sampleRate());
    if(!device.isEmpty())
        setOutputDeviceName(device);
    if(!outputFormat.isValid()||outputDevice.isNull()){
        qDebug()<<"play failed,sample rate:"<<outputFormat.sampleRate()
               <<"device null:"<<outputDevice.isNull()<<outputDevice.supportedSampleRates();
        return false;
    }
    //QAudioFormat n_format=outputDevice.nearestFormat(outputFormat);
    audioOutput=new QAudioOutput(outputDevice,outputFormat,this);
    connect(audioOutput,&QAudioOutput::stateChanged,this,&AudioRecorderOutput::stateChanged);
    audioOutput->start(io);
    return true;
}

void AudioRecorderOutput::stopPlay()
{
    if(audioOutput){
        audioOutput->stop();
        audioOutput->deleteLater();
        audioOutput=nullptr;
    }
}

bool AudioRecorderOutput::saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath)
{
    //stopPlay();
    if(data.isEmpty())
        return false;
    //qfile不能生成目录
    QFileInfo info(filepath);
    if(!info.dir().exists())
        info.dir().mkpath(info.absolutePath());

    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly)){
        return false;
    }

    //暂时全部写入
    AudioRecorderWavHead head=AudioRecorderWavHead::createWavHead(format.sampleRate(),data.size());
    file.write((const char *)(&head),sizeof(AudioRecorderWavHead));
    file.write(data);
    file.close();
    return true;
}
