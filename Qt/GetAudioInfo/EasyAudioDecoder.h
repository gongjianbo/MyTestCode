#ifndef EASYAUDIODECODER_H
#define EASYAUDIODECODER_H

#include <functional>

#include "EasyAudioContext.h"

/**
 * @brief 处理音频解码相关
 * @author 龚建波
 * @date 2020-11-20
 * @details
 * 主要功能为将音频数据解码为PCM
 * （本来只处理编解码，不带多线程处理部分）
 *
 * 重采样参考：
 * ffmpeg-4.2.4\doc\examples\resampling_audio.c
 * https://www.jianshu.com/p/bf5e54f553a4
 */
class EasyAudioDecoder
{
public:
    explicit EasyAudioDecoder(const QString &filepath);
    explicit EasyAudioDecoder(EasyAudioContext *context);
    explicit EasyAudioDecoder(const QSharedPointer<EasyAudioContext> &context);

    //根据绑定的上下文获取pcm数据(使用重采样libswresample)
    //params:目标格式的参数
    //callBack:转换时的同步回调函数
    // 每次packet处理都会调用，若返回false则整个toPcm无效返回false
    // 回调函数参描1为输出缓冲区地址，参数2为输出数据有效字节长度
    //return false:表示转换无效失败
    bool toPcm(const EasyAudioParameter &params,std::function<bool(const char* outData,int outSize)> callBack);

    //根据绑定的上下文获取pcm数据(使用重采样libswresample)
    //params:目标格式的参数
    //返回数据的size为0则转换无效
    QByteArray toPcmData(const EasyAudioParameter &params);

private:
    //上下文
    QSharedPointer<EasyAudioContext> contextPtr;

    //存储压缩数据
    //视频通常包含一个压缩帧，音频可能包含多个压缩帧
    AVPacket *packet = NULL;
    //描述原始数据
    AVFrame *frame = NULL;
    //重采样上下文
    SwrContext *swrCtx = NULL;
};

#endif // EASYAUDIODECODER_H
