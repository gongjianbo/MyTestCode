#pragma once
#include <QString>
#include <QSharedPointer>

//在头文件导入只是偷个懒
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswresample/swresample.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
}

/**
 * @brief 存储音频格式等信息
 */
struct EasyAudioInfo
{
    bool valid=false;

    //下面为音频文件相关信息
    QString filepath;
    QString filename;
    QString encode;
    int sampleRate = 0;
    int channels = 0;
    int sampleBit = 0;
    qint64 duration = 0;
    qint64 bitRate = 0;
    qint64 size = 0;
    QString type;
};

/**
 * @brief 存储输入或者输出用的参数
 */
struct EasyAudioParameter
{
    //通道数
    int channels=1;
    //采样存储格式，对应枚举AVSampleFormat，AV_SAMPLE_FMT_S16=1
    AVSampleFormat sampleFormat=AV_SAMPLE_FMT_S16;
    //采样率
    int sampleRate=16000;
};

/**
 * @brief 管理音频上下文，也可用来获取音频格式等信息
 * @author 龚建波
 * @date 2020-11-20
 * @details
 * 去掉了拷贝和赋值，需要作为参数传递时使用智能指针管理
 * （为什么用 NULL 不用 nullptr，为了和 C 保持一致）
 *
 * 内存管理参考：
 * https://www.jianshu.com/p/9f45d283d904
 * https://blog.csdn.net/leixiaohua1020/article/details/41181155
 *
 * 获取音频信息参考：
 * https://blog.csdn.net/zhoubotong2012/article/details/79340722
 * https://blog.csdn.net/luotuo44/article/details/54981809
 *
 */
class EasyAudioContext
{
private:
    //判断解析状态，只有Success才表示成功
    enum EasyState{
        None //无效的
        ,Success //解析成功
        ,NoFile //文件不存在
        ,FormatOpenError //打开文件失败
        ,FindStreamError //读取流信息失败
        ,NoAudio //未找到音频流
        ,CodecFindDecoderError //未找到解码器
        ,CodecAllocContextError //分配解码上下文失败
        ,ParameterError //填充解码上下文失败
        ,CodecOpenError //打开解码器失败
    };
public:
    //根据文件创建上下文对象
    explicit EasyAudioContext(const QString &filepath);
    //去掉了拷贝和赋值，使用智能指针管理
    EasyAudioContext(const EasyAudioContext &other)=delete;
    EasyAudioContext &operator =(const EasyAudioContext &other)=delete;
    EasyAudioContext(EasyAudioContext &&other)=delete;
    EasyAudioContext &operator =(EasyAudioContext &&other)=delete;
    //析构中释放资源
    ~EasyAudioContext();

    //是否为有效的上下文
    bool isValid() const;
    //获取该上下文音频格式等信息
    EasyAudioInfo getAudioInfo() const;
    //获取该上下文参数信息
    EasyAudioParameter getAudioParameter() const;

private:
    //根据文件初始化上下文
    void init(const QString &filepath);
    //释放资源
    void free();

private:
    //源文件路径
    QString srcpath;
    //该上下文是否有效，默认无效
    EasyState status=None;

    //格式化I/O上下文
    AVFormatContext *formatCtx = NULL;
    //解码器
    AVCodec *codec = NULL;
    //解码器上下文
    AVCodecContext *codecCtx = NULL;
    //参数信息
    AVCodecParameters *codecParam = NULL;
    //音频流index
    int streamIndex = -1;

    //在友元中访问私有变量用
    friend class EasyAudioDecoder;
};
