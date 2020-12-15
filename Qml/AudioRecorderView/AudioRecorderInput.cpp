#include "AudioRecorderInput.h"

#include <QFile>
#include <QDebug>

AudioRecorderInput::AudioRecorderInput(QObject *parent)
    : QObject(parent)
{
    //采样精度和声道数暂时默认
    inputFormat.setSampleRate(16000);
    inputFormat.setChannelCount(1);
    inputFormat.setSampleSize(16);
    inputFormat.setCodec("audio/pcm");
    inputFormat.setByteOrder(QAudioFormat::LittleEndian);
    inputFormat.setSampleType(QAudioFormat::SignedInt);

    updateInputDevices();
    resetToDefaultDevice();
    //if(filterInputDevices.count()>0)
    //    inputDevice=filterInputDevices.first();
}

AudioRecorderInput::~AudioRecorderInput()
{
    freeRecord();
}

void AudioRecorderInput::updateInputDevices()
{
    allInputDevices=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    filterInputDevices.clear();

    //根据采样率过滤输入设备
    for(auto &info:allInputDevices)
    {
        if(info.supportedSampleRates().contains(inputFormat.sampleRate())){
            filterInputDevices.push_back(info);
        }
    }
    emit filterInputDevicesNameChanged();
}

bool AudioRecorderInput::checkInputExists() const
{
    //插拔设备后，deviceInfo不相等了
    for(auto &info:QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        if(info.deviceName()==audioDevice.deviceName()
                &&info.supportedSampleRates()==audioDevice.supportedSampleRates()){
            return true;
        }
    }
    return false;
    //return QAudioDeviceInfo::availableDevices(QAudio::AudioInput).contains(audioDevice);
}

void AudioRecorderInput::resetToDefaultDevice()
{
    inputDevice=QAudioDeviceInfo::defaultInputDevice();
}

QStringList AudioRecorderInput::getAllInputDevicesName() const
{
    QStringList name_list;
    for(auto &info:allInputDevices)
        name_list.push_back(info.deviceName());
    return name_list;
}

QStringList AudioRecorderInput::getFilterInputDevicesName() const
{
    QStringList name_list;
    for(auto &info:filterInputDevices)
        name_list.push_back(info.deviceName());
    return name_list;
}

QString AudioRecorderInput::getInputDeviceName() const
{
    return inputDevice.deviceName();
}

void AudioRecorderInput::setInputDeviceName(const QString &device)
{
    inputDevice=QAudioDeviceInfo();
    for(auto &info:filterInputDevices)
    {
        if(info.deviceName()==device){
            inputDevice=info;
            break;
        }
    }
}

void AudioRecorderInput::setInputDeviceIndex(int index)
{
    if(index<0||index>=filterInputDevices.count())
        return;
    inputDevice=filterInputDevices.at(index);
}

bool AudioRecorderInput::isAvailableDevice(const QString &device)
{
    QList<QAudioDeviceInfo> device_list=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(auto &info:device_list)
    {
        if(info.deviceName()==device&&filterInputDevices.contains(info))
            return true;
    }
    return false;
}

bool AudioRecorderInput::startRecord(AudioRecorderDevice *io, const QAudioFormat &format, const QString &device)
{
    stopRecord();

    if(format.sampleRate()>0)
        inputFormat.setSampleRate(format.sampleRate());
    if(!device.isEmpty())
        setInputDeviceName(device);
    if(!inputFormat.isValid()||inputDevice.isNull()){
        qDebug()<<"record failed,sample rate:"<<inputFormat.sampleRate()
               <<"device null:"<<inputDevice.isNull()<<inputDevice.supportedSampleRates();
        return false;
    }

    //参数不相等才重新new
    if(audioInput&&(audioDevice!=inputDevice||audioInput->format()!=inputFormat)){
        freeRecord();
    }
    if(!audioInput){
        //qDebug()<<"new audioInput";
        //保存当前deviceinfo，下次对比是否相同
        audioDevice=inputDevice;
        audioInput=new QAudioInput(inputDevice,inputFormat,this);
        connect(audioInput,&QAudioInput::stateChanged,this,&AudioRecorderInput::stateChanged);
        connect(audioInput,&QAudioInput::notify,this,&AudioRecorderInput::notify);
    }
    audioInput->start(io);
    return true;
}

void AudioRecorderInput::stopRecord()
{
    if(audioInput){
        audioInput->stop();
    }
}

void AudioRecorderInput::freeRecord()
{
    if(audioInput){
        audioInput->stop();
        audioInput->deleteLater();
        audioInput=nullptr;
    }
}

bool AudioRecorderInput::loadFromFile(AudioRecorderDevice *io,const QString &filepath)
{
    stopRecord();
    QFile file(filepath);
    if(file.exists()&&file.size()>44&&
            file.open(QIODevice::ReadOnly)){
        AudioRecorderWavHead head;
        file.read((char*)&head,44);
        QByteArray pcm_data;
        if(AudioRecorderWavHead::isValidWavHead(head)){
            //暂时为全部读取
            pcm_data=file.readAll();
            file.close();
        }
        //采样率等置为相同参数
        if(pcm_data.count()>0&&pcm_data.count()%2==0){
            inputFormat.setSampleRate(head.sampleRate);
            inputFormat.setChannelCount(head.numChannels);
            inputFormat.setSampleSize(head.bitsPerSample);
            io->writeData(pcm_data.constData(),pcm_data.count());
            return true;
        }
    }
    return false;
}

