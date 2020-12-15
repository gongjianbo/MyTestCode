#ifndef AUDIORECORDERDEVICE_H
#define AUDIORECORDERDEVICE_H

#include <QAudio>
#include <QAudioFormat>
#include <QIODevice>

/**
 * @brief wav文件头结构体
 * @author 龚建波
 * @date 2020-11-12
 * @details
 * wav头是不定长格式，不过这里用的比较简单的格式
 * （数值以小端存储，不过pc一般是小端存储，暂不特殊处理）
 * 参照：https://www.cnblogs.com/ranson7zop/p/7657874.html
 * 参照：https://www.cnblogs.com/Ph-one/p/6839892.html
 * @note
 * 最大元素为4字节，所以本身就是44字节对齐的
 * 加字节对齐只是为了提醒
 */
#pragma pack(push,1)
struct AudioRecorderWavHead
{
    char riffFlag[4]; //文档标识，大写"RIFF"
    //从下一个字段首地址开始到文件末尾的总字节数。
    //该字段的数值加 8 为当前文件的实际长度。
    unsigned int riffSize; //数据长度
    char waveFlag[4]; //文件格式标识，大写"WAVE"
    char fmtFlag[4]; //格式块标识，小写"fmt "
    unsigned int fmtSize; //格式块长度，可以是 16、 18 、20、40 等
    unsigned short compressionCode; //编码格式代码，1为pcm
    unsigned short numChannels; //通道个数
    unsigned int sampleRate; //采样频率
    //该数值为:声道数×采样频率×每样本的数据位数/8。
    //播放软件利用此值可以估计缓冲区的大小。
    unsigned int bytesPerSecond; //码率（数据传输速率）
    //采样帧大小。该数值为:声道数×位数/8。
    //播放软件需要一次处理多个该值大小的字节数据,用该数值调整缓冲区。
    unsigned short blockAlign; //数据块对其单位
    //存储每个采样值所用的二进制数位数。常见的位数有 4、8、12、16、24、32
    unsigned short bitsPerSample; //采样位数（采样深度）
    char dataFlag[4]; //表示数据开头，小写"data"
    unsigned int dataSize; //数据部分的长度

    //生成wav(pcm)文件头信息
    //（参数列表根据使用来修改）
    //[in] sampleRate: 采样频率
    //[in] dataSize: pcm数据字节长度
    //[return] EasyWavHead: wav头
    static AudioRecorderWavHead createWavHead(int sampleRate,unsigned int dataSize);

    //检测该头数据是否有效
    static bool isValidWavHead(const AudioRecorderWavHead &head);
};
#pragma pack(pop)

/**
 * @brief 用于AudioRecorderDevice回调
 * @author 龚建波
 * @date 2020-12-4
 * @details
 * 实际存储数据的类继承该类，在device类回调时调用虚函数
 */
class AudioRecorderBase
{
public:
    AudioRecorderBase()=default;
    virtual ~AudioRecorderBase()=0 {}
    //qint64 readData(char *data, qint64 maxSize)
    virtual qint64 readData(char *, qint64 ) { return -1; }
    //qint64 writeData(const char *data, qint64 maxSize)
    virtual qint64 writeData(const char *, qint64 ) { return -1; }
};

/**
 * @brief QAudioInput等处理数据时的辅助类
 * @author 龚建波
 * @date 2020-12-4
 */
class AudioRecorderDevice : public QIODevice
{
public:
    explicit AudioRecorderDevice(AudioRecorderBase *base,QObject *parent = nullptr);

    //需函数接口用于数据读写回调
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    //读写数据时再回调view相关接口进行操作
    AudioRecorderBase *basePtr=nullptr;
};


#endif // AUDIORECORDERDEVICE_H
