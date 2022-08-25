#pragma once
#include <functional>
#include "EasyAudioContext.h"

/**
 * @brief wav文件头结构体
 * @author 龚建波
 * @date 2020-11-12
 * @details
 * wav头是不定长格式，不过这里用的比较简单的格式
 * （数值以小端存储，不过pc一般是小端存储，暂不特殊处理）
 * 参照：https://www.cnblogs.com/ranson7zop/p/7657874.html
 * 参照：https://www.cnblogs.com/Ph-one/p/6839892.html
 */
struct EasyWavHead
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
};

/**
 * @brief 处理音频解码相关
 * @author 龚建波
 * @date 2020-11-24
 * @details
 * 主要功能为将音频数据解码为PCM
 * （本来只处理编解码，不带多线程处理部分）
 *
 * 测试文件地址：
 * https://samples.mplayerhq.hu/A-codecs/
 * 音乐下载：
 * http://www.musictool.top/
 * 重采样参考：
 * ffmpeg-4.2.4\doc\examples\resampling_audio.c
 * https://www.jianshu.com/p/bf5e54f553a4
 * https://segmentfault.com/a/1190000025145553
 * https://blog.csdn.net/bixinwei22/article/details/86545497
 * https://blog.csdn.net/zhuweigangzwg/article/details/53395009
 */
class EasyAudioDecoder
{
public:
    //解码单个音频文件，获取pcm数据
    //filepath:文件路径
    //params:目标格式的参数
    //返回数据的size为0则转换无效
    static QByteArray toPcmData(const QString &filepath,
                         const EasyAudioParameter &params);

    //转码音频文件为指定参数格式的wav(pcm)
    //filepath:原文件路径
    //dstpath:目标文件路径
    //params:目标格式的参数
    //返回false则转换失败
    static bool transcodeToWav(const QString &srcpath,
                        const QString &dstpath,
                        const EasyAudioParameter &params);

    //多个文件转码并拼接为指定参数格式的wav(pcm)
    //bool stitchingToWav(const QList<QString> &srcpaths,
    //                    const QString &dstpath,
    //                    const EasyAudioParameter &params);


    //获取pcm数据(使用重采样libswresample)
    //contextPtr:上下文指针
    //params:目标格式的参数，如果参数无效会使用原数据参数
    //callBack:转换时的同步回调函数
    // 每次packet处理都会调用，若返回false则整个toPcm无效返回false
    // 回调函数参描1为输出缓冲区地址，参数2为输出数据有效字节长度
    //return false:表示转换无效失败
    static bool toPcm(const QSharedPointer<EasyAudioContext> &contextPtr,
                      const EasyAudioParameter &params,
                      std::function<bool(const char* outData,int outSize)> callBack);
    //对智能指针版本的封装
    static bool toPcm(const QString &filepath,
                      const EasyAudioParameter &params,
                      std::function<bool(const char* outData,int outSize)> callBack);

    //生成wav(pcm)文件头信息
    //
    //sampleRate: 采样频率
    //channels: 通道数，一般为1
    //sampleFormat: AVSampleFormat枚举值
    //dataSize: pcm数据字节长度
    //return EasyWavHead: wav头
    static EasyWavHead createWavHead(int sampleRate,int channels,AVSampleFormat sampleFormat,unsigned int dataSize);

    //判断导出参数是否有效，在无效的情况下将使用输出参数对应字段值
    static EasyAudioParameter getOutParameter( const EasyAudioParameter &inParams,const EasyAudioParameter &outParams);
};
