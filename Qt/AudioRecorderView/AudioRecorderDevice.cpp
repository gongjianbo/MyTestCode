#include "AudioRecorderDevice.h"

const char *RIFF_FLAG="RIFF";
const char *WAVE_FLAG="WAVE";
const char *FMT_FLAG="fmt ";
const char *DATA_FLAG="data";

AudioRecorderWavHead AudioRecorderWavHead::createWavHead(int sampleRate, unsigned int dataSize)
{
    //部分参数暂时用的固定值
    const int bits=16;
    const int channels=1;
    const int head_size = sizeof(AudioRecorderWavHead);
    AudioRecorderWavHead wav_head;
    memset(&wav_head, 0, head_size);

    memcpy(wav_head.riffFlag, RIFF_FLAG, 4);
    memcpy(wav_head.waveFlag, WAVE_FLAG, 4);
    memcpy(wav_head.fmtFlag, FMT_FLAG, 4);
    memcpy(wav_head.dataFlag, DATA_FLAG, 4);

    //出去头部前8个字节的长度，用的44字节的格式头，所以+44-8=36
    wav_head.riffSize = dataSize + 36;
    //不知道干嘛的
    wav_head.fmtSize = 16;
    //1为pcm
    wav_head.compressionCode = 0x01;
    wav_head.numChannels = channels;
    wav_head.sampleRate = sampleRate;
    wav_head.bytesPerSecond = (bits / 8) * channels * sampleRate;
    wav_head.blockAlign = (bits / 8) * channels;
    wav_head.bitsPerSample = bits;
    //出去头的数据长度
    wav_head.dataSize = dataSize;

    return wav_head;
}

bool AudioRecorderWavHead::isValidWavHead(const AudioRecorderWavHead &head)
{
    //简单的比较，主要用在未使用解析器时解析wav头
    if(memcmp(head.riffFlag, RIFF_FLAG, 4)!=0||
            memcmp(head.waveFlag, WAVE_FLAG, 4)!=0||
            memcmp(head.fmtFlag, FMT_FLAG, 4)!=0||
            memcmp(head.dataFlag, DATA_FLAG, 4)!=0||
            head.riffSize!=head.dataSize+36||
            head.compressionCode!=0x01)
        return false;
    return true;
}

AudioRecorderDevice::AudioRecorderDevice(AudioRecorderBase *base, QObject *parent)
    : QIODevice(parent),basePtr(base)
{
    Q_ASSERT(base!=nullptr);
}

qint64 AudioRecorderDevice::readData(char *data, qint64 maxSize)
{
    return basePtr->readData(data,maxSize);
}

qint64 AudioRecorderDevice::writeData(const char *data, qint64 maxSize)
{
    return basePtr->writeData(data,maxSize);
}

