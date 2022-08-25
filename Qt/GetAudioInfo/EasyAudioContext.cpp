#include "EasyAudioContext.h"

#include <QFileInfo>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include "libswresample/swresample.h"
#include <libavutil/frame.h>
#include <libavutil/mem.h>
}

EasyAudioContext::EasyAudioContext(const QString &filepath)
{
    init(filepath);
}

EasyAudioContext::~EasyAudioContext()
{
    free();
}

bool EasyAudioContext::isValid() const
{
    return (status==EasyState::Success);
}

EasyAudioInfo EasyAudioContext::getAudioInfo() const
{
    EasyAudioInfo info;

    //把需要的格式信息copy过来
    info.valid = isValid();
    info.filepath = srcpath;
    QFileInfo f_info(srcpath);
    info.filename = f_info.fileName();
    info.size = f_info.size();

    if(!isValid())
        return info;

    info.encode = codec->name;
    info.sampleRate = codecParam->sample_rate; //hz
    info.channels = codecParam->channels;
    //2022-08-25 之前取的采样精度不是文件实际的精度，导致24bit等不能正确识别
    //info.sampleBit = (av_get_bytes_per_sample(codecCtx->sample_fmt)<<3);  //bit
    info.sampleBit = av_get_bits_per_sample(codecParam->codec_id);
    //if (codecCtx && codecCtx->bits_per_raw_sample > 0) {
    //    info.sampleBit = codecCtx->bits_per_raw_sample;
    //}
    info.duration = formatCtx->duration/(AV_TIME_BASE/1000.0);  //ms
    //2020-12-31 测试一个ape文件时发现音频信息比特率为0，现判断无效则使用容器比特率
    info.bitRate = codecCtx->bit_rate<1?formatCtx->bit_rate:codecCtx->bit_rate; //bps
    info.type = formatCtx->iformat->name;

    return info;
}

EasyAudioParameter EasyAudioContext::getAudioParameter() const
{
    EasyAudioParameter param;

    if(!isValid())
        return param;
    param.channels=codecCtx->channels;
    param.sampleFormat=codecCtx->sample_fmt;
    param.sampleRate=codecCtx->sample_rate;

    return param;
}

void EasyAudioContext::init(const QString &filepath)
{
    srcpath=filepath;
    if(!QFileInfo::exists(filepath)){
        status=EasyState::NoFile;
        return;
    }

    //ffmpeg默认用的utf8编码，这里转换下
    QByteArray temp=filepath.toUtf8();
    const char *path=temp.constData();
    //const char *filepath="D:/Download/12.wav";

    //打开输入流并读取头
    //流要使用avformat_close_input关闭
    //成功时返回=0
    const int result=avformat_open_input(&formatCtx, path, NULL, NULL);
    if (result!=0||formatCtx==NULL){
        status=EasyState::FormatOpenError;
        return;
    }

    //读取文件获取流信息，把它存入AVFormatContext中
    //正常时返回>=0
    if (avformat_find_stream_info(formatCtx, NULL) < 0) {
        status=EasyState::FindStreamError;
        return;
    }

    //qDebug()<<"filepath"<<filepath;
    //duration/AV_TIME_BASE单位为秒
    //qDebug()<<"duration"<<formatCtx->duration/(AV_TIME_BASE/1000.0)<<"ms";
    //文件格式，如wav
    //qDebug()<<"format"<<formatCtx->iformat->name<<":"<<formatCtx->iformat->long_name;
    //qDebug()<<"bitrate"<<formatCtx->bit_rate<<"bps";
    //qDebug()<<"n stream"<<formatCtx->nb_streams;

    status=EasyState::NoAudio;
    for (unsigned int i = 0; i < formatCtx->nb_streams; i++)
    {
#if 1
        //AVStream是存储每一个视频/音频流信息的结构体
        AVStream *in_stream = formatCtx->streams[i];

        //类型为音频
        if(in_stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            codecParam = in_stream->codecpar;
            streamIndex = i;
            //查找具有匹配编解码器ID的已注册解码器
            //失败返回NULL
            codec = avcodec_find_decoder(in_stream->codecpar->codec_id);
            if(codec==NULL){
                status=EasyState::CodecFindDecoderError;
                return;
            }

            //分配AVCodecContext并将其字段设置为默认值
            //需要使用avcodec_free_context释放生成的对象
            //如果失败，则返回默认填充或者 NULL
            codecCtx = avcodec_alloc_context3(codec);
            if(codecCtx==NULL){
                status=EasyState::CodecAllocContextError;
                return;
            }

            //根据编码器填充上下文参数
            //事实上codecpar包含了大部分解码器相关的信息，这里是直接从AVCodecParameters复制到AVCodecContext
            //成功时返回值>=0
            if(avcodec_parameters_to_context(codecCtx, in_stream->codecpar)<0){
                status=EasyState::ParameterError;
                return;
            }

            //某些AVCodecContext字段的访问器，已弃用
            //av_codec_set_pkt_timebase(codec_ctx, in_stream->time_base);

            //没有此句会出现：Could not update timestamps for skipped samples
            codecCtx->pkt_timebase = formatCtx->streams[i]->time_base;

            //打开解码器（将解码器和解码器上下文进行关联）
            //使用给定的AVCodec初始化AVCodecContext
            //在之前必须使用avcodec_alloc_context3()分配上下文
            //成功时返回值=0
            if(avcodec_open2(codecCtx, codec, nullptr)!=0){
                status=EasyState::CodecOpenError;
                return;
            }

            //采样率
            //qDebug()<<"sample rate"<<codecCtx->sample_rate;
            //编码，如pcm
            //qDebug()<<"codec name"<<codec->name<<":"<<codec->long_name;

            status=EasyState::Success;
            return;
        }
#else
        //新的版本这种获取方式已弃用
        AVStream *in_stream = fmt_ctx->streams[i];
        AVCodecContext *avctx=in_stream->codec;
        if (avctx->codec_type == AVMEDIA_TYPE_VIDEO){
            //视频信息略
        }else if (avctx->codec_type == AVMEDIA_TYPE_AUDIO){
            //音频信息
            qDebug()<<"sample rate"<<in_stream->codec->sample_rate;
            AVCodec *codec=avcodec_find_decoder(avctx->codec_id);
            if(codec==NULL){
                return;
            }
            qDebug()<<"codec name"<<codec->name<<":"<<codec->long_name;
            return;
        }
#endif
    }
}

void EasyAudioContext::free()
{
    if(codecCtx){
        //不要直接使用avcodec_close，而是用avcodec_free_context
        //把codec相关的其他东西一并释放
        avcodec_free_context(&codecCtx);
    }
    if(formatCtx){
        //avformat_close_input内部其实已经调用了avformat_free_context
        avformat_close_input(&formatCtx);
        //avformat_free_context(formatCtx);
    }
    codec=NULL;
    codecCtx=NULL;
    codecParam=NULL;
    formatCtx=NULL;
}
