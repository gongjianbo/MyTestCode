#include "EasyAudioDecoder.h"

#include <QScopeGuard>
#include <QDebug>

EasyAudioDecoder::EasyAudioDecoder(const QString &filepath)
    : contextPtr(new EasyAudioContext(filepath))
{}

EasyAudioDecoder::EasyAudioDecoder(EasyAudioContext *context)
    : contextPtr(context)
{}

EasyAudioDecoder::EasyAudioDecoder(const QSharedPointer<EasyAudioContext> &context)
    : contextPtr(context)
{}

bool EasyAudioDecoder::toPcm(const EasyAudioParameter &params, std::function<bool (const char *, int)> callBack)
{
    //描述存储压缩数据
    //视频通常包含一个压缩帧，音频可能包含多个压缩帧
    AVPacket *packet = NULL;
    //描述原始数据
    AVFrame *frame = NULL;
    //重采样上下文
    SwrContext *swr_ctx = NULL;
    //解析时out缓冲
    int out_bufsize=1024*1024; //默认1M大小
    uint8_t *out_buffer = (uint8_t*)av_malloc(out_bufsize);
    if(!out_buffer)
        return false;

    //没有变量来接收的话会立即执行
    auto clean=qScopeGuard([&]{
        if(packet){
            //av_free_packet改用av_packet_unref
            av_packet_unref(packet);
            av_packet_free(&packet);
        }
        if(frame){
            av_frame_unref(frame);
            av_frame_free(&frame);
        }
        if(swr_ctx){
            swr_close(swr_ctx);
            swr_free(&swr_ctx);
        }
        if (out_buffer)
            av_freep(&out_buffer);
        qDebug()<<"toPcm clean";
    });
    Q_UNUSED(clean);

    if(contextPtr.isNull()||!contextPtr->isValid())
        return false;

    packet=av_packet_alloc();
    av_init_packet(packet);
    frame=av_frame_alloc();

    //目标格式的参数
    const int out_channels = params.channels;
    const AVSampleFormat out_sample_fmt=params.sampleFormat;
    const int out_sample_rate=params.sampleRate;
    //返回每个sample的字节数
    const int sample_bytes=av_get_bytes_per_sample(out_sample_fmt);

    //分配SwrContext并设置/重置公共参数
    //返回NULL为失败，否则分配上下文
    //（目前的需求默认是转为单声道 16bit 的，只有采样率会设置）
    swr_ctx=swr_alloc_set_opts(NULL, //现有的swr上下文，不可用则为NULL
                               av_get_default_channel_layout(out_channels), //输出通道布局 (AV_CH_LAYOUT_*)
                               out_sample_fmt, //输出采样格式 (AV_SAMPLE_FMT_*).
                               out_sample_rate, //输出采样频率 (frequency in Hz)
                               av_get_default_channel_layout(contextPtr->codecCtx->channels), //输入通道布局 (AV_CH_LAYOUT_*)
                               contextPtr->codecCtx->sample_fmt, //输入采样格式 (AV_SAMPLE_FMT_*).
                               contextPtr->codecCtx->sample_rate, //输入采样频率 (frequency in Hz)
                               0, NULL); //日志相关略
    if(swr_ctx==NULL)
        return false;

    //初始化
    //如果要修改转换的参数，调用参数设置后再次init
    if(swr_init(swr_ctx)<0)
        return false;

    //因为av_read_frame后，下次就没法继续调用，为了重入我先把seek置位
    //参数一: s为容器内容;
    //参数二: 流索引, 如果stream_index是-1，会选择一个默认流，时间戳会从以AV_TIME_BASE为单位向具体流的时间基自动转换。
    //参数三: 将要定位处的时间戳，time_base单位或者如果没有流是指定的就用av_time_base单位。
    //参数四: seek功能flag；
    //AVSEEK_FLAG_BACKWARD  是seek到请求的timestamp之前最近的关键帧
    //AVSEEK_FLAG_BYTE 是基于字节位置的查找
    //AVSEEK_FLAG_ANY 是可以seek到任意帧，注意不一定是关键帧，因此使用时可能会导致花屏
    //AVSEEK_FLAG_FRAME 是基于帧数量快进
    //返回值：成功返回>=0
    if(av_seek_frame(contextPtr->formatCtx,-1,AV_TIME_BASE,AVSEEK_FLAG_ANY)<0)
        return false;

    int ret=0;

    //返回流的下一帧
    //返回0表示成功
    while (av_read_frame(contextPtr->formatCtx, packet)==0)
    {
        //取音频
        if (packet->stream_index == contextPtr->audioStreamIndex)
        {
            //提供原始数据作为解码器的输入(将packet写入到解码队列当中去)
            //返回0表示成功
            ret=avcodec_send_packet(contextPtr->codecCtx, packet);
            if(ret!=0)
                continue;

            while (ret >= 0) {
                //从解码器返回解码输出数据
                //返回0表示成功
                ret = avcodec_receive_frame(contextPtr->codecCtx, frame);

                if (ret!=0) {
                    break;
                }
                //下一个采样数的上限
                const int out_samples=swr_get_out_samples(swr_ctx,frame->nb_samples);
                //缓冲区大小是否足够，不够就根据计算值扩充
                if(out_bufsize<sample_bytes*out_samples){
                    av_freep(&out_buffer);
                    out_buffer=(uint8_t*)av_malloc(sample_bytes*out_samples*1.5);
                    if(!out_buffer){
                        av_frame_unref(frame);
                        av_packet_unref(packet);
                        return false;
                    }
                }

                //重采样转换
                //如果提供的输入多于输出空间，则输入将被缓冲。
                //通过使用swr_get_out_samples来获取下一次调用swr_convert在给定输入样本数量下输出样本数量的上限，来提供足够的空间。
                //只要有可能，转换将直接运行而不进行复制。
                //return每个通道输出的样本数，出错时为负值
                ret = swr_convert(swr_ctx, &out_buffer, out_samples,
                                      (const uint8_t **)frame->data,
                                      frame->nb_samples);

                if (ret <= 0) {
                    av_frame_unref(frame);
                    continue;
                }

                //获取给定音频参数所需的缓冲区大小
                const int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, ret, out_sample_fmt, 1);
                if(out_channels==2)
                {
                    //S16左右声道各两个字节
                    for(int i = 0; i < out_buffer_size; i += sample_bytes*2)
                    {
                        //提取左声道数据
                        //pcm_data.append((const char*)out_buffer + i, sample_bytes);
                        //回调false则整体失败返回false
                        if(!callBack((const char*)out_buffer + i, sample_bytes)){
                            av_frame_unref(frame);
                            av_packet_unref(packet);
                            return false;
                        }
                    }
                }else if(out_channels==1){
                    //单声道数据
                    //pcm_data.append((const char*)out_buffer, out_buffer_size);
                    //回调false则整体失败返回false
                    if(!callBack((const char*)out_buffer, out_buffer_size)){
                        av_frame_unref(frame);
                        av_packet_unref(packet);
                        return false;
                    }
                }
                av_frame_unref(frame);
            }
        }
        av_packet_unref(packet);
    }
    qDebug()<<"toPcm end";
    return true;
}

QByteArray EasyAudioDecoder::toPcmData(const EasyAudioParameter &params)
{
    //保存解码后的pcm数据
    QByteArray pcm_data;
    //toPcm的回调参数
    std::function<bool(const char*,int)> call_back=[&](const char* pcmData,int pcmSize)
    {
        pcm_data.append(pcmData, pcmSize);
        return true;
    };

    if(!toPcm(params,call_back))
        pcm_data.clear();
    return pcm_data;
}

