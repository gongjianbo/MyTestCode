#include "AudioRecorderOutput.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
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

    updateOutputDevices();
    resetToDefaultDevice();
    //if(filterOutputDevices.count()>0)
    //    outputDevice=filterOutputDevices.first();

    setCacheDir(qApp->applicationDirPath()+"/AppData/Default/AudioRecorder");
}

AudioRecorderOutput::~AudioRecorderOutput()
{
    freePlay();
}

void AudioRecorderOutput::setCacheDir(const QString &dir)
{
    if(cacheDir!=dir){
        cacheDir=dir;
        emit cacheDirChanged(cacheDir);
    }
}

void AudioRecorderOutput::updateOutputDevices()
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

bool AudioRecorderOutput::checkOutputExists() const
{
    //插拔设备后，deviceInfo不相等了
    for(auto &info:QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        if(info.deviceName()==audioDevice.deviceName()
                &&info.supportedSampleRates()==audioDevice.supportedSampleRates()){
            return true;
        }
    }
    return false;
    //return QAudioDeviceInfo::availableDevices(QAudio::AudioOutput).contains(audioDevice);
}

void AudioRecorderOutput::resetToDefaultDevice()
{
    outputDevice=QAudioDeviceInfo::defaultOutputDevice();
}

QStringList AudioRecorderOutput::getAllOutputDevicesName() const
{
    QStringList name_list;
    for(auto &info:allOutputDevices)
        name_list.push_back(info.deviceName());
    return name_list;
}

QStringList AudioRecorderOutput::getFilterOutputDevicesName() const
{
    QStringList name_list;
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

    //参数不相等才重新new
    if(audioOutput&&(audioDevice!=outputDevice||audioOutput->format()!=outputFormat)){
        freePlay();
    }
    if(!audioOutput){
        //qDebug()<<"new audioOutput";
        //保存当前deviceinfo，下次对比是否相同
        audioDevice=outputDevice;
        //QAudioFormat n_format=outputDevice.nearestFormat(outputFormat);
        audioOutput=new QAudioOutput(outputDevice,outputFormat,this);
        connect(audioOutput,&QAudioOutput::stateChanged,this,&AudioRecorderOutput::stateChanged);
        connect(audioOutput,&QAudioOutput::notify,this,&AudioRecorderOutput::notify);
        //目前用notify来控制进度刷新
        audioOutput->setNotifyInterval(50);
    }
    audioOutput->start(io);
    return true;
}

void AudioRecorderOutput::stopPlay()
{
    if(audioOutput){
        audioOutput->stop();
    }
}

void AudioRecorderOutput::suspendPlay()
{
    if(audioOutput){
        audioOutput->suspend();
    }
}

void AudioRecorderOutput::resumePlay()
{
    if(audioOutput){
        audioOutput->resume();
    }
}

void AudioRecorderOutput::freePlay()
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

QString AudioRecorderOutput::saveToCache(const QByteArray data, const QAudioFormat &format, const QString &uuid)
{
    const QString file_path=QString("%1/%2.wav").arg(getCacheDir()).arg(uuid);
    if(saveToFile(data,format,file_path))
        return file_path;
    return QString();
}
