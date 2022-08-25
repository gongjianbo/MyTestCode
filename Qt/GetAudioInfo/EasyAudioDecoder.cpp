#include "EasyAudioDecoder.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QScopeGuard>
#include <QDebug>

QByteArray EasyAudioDecoder::toPcmData(const QString &filepath, const EasyAudioParameter &params)
{
    //保存解码后的pcm数据
    QByteArray pcm_data;
    //toPcm的回调参数
    std::function<bool(const char*,int)> call_back=[&](const char* pcmData,int pcmSize)
    {
        pcm_data.append(pcmData, pcmSize);
        return true;
    };

    if(!toPcm(filepath,params,call_back))
        pcm_data.clear();
    return pcm_data;
}

bool EasyAudioDecoder::transcodeToWav(const QString &srcpath, const QString &dstpath, const EasyAudioParameter &params)
{
    qDebug()<<"transcodeToWav begin. src"<<srcpath<<"dst"<<dstpath;
    bool trans_result=false;
    QFileInfo src_info(srcpath);
    QFileInfo dst_info(dstpath);
    if(!src_info.exists())
        return trans_result;
    //判断目录存在(qfile不能生成目录)
    if(dst_info.dir().exists()||dst_info.dir().mkpath(dst_info.absolutePath())){
        EasyWavHead head;

        QFile dst_file(dstpath);
        if(dst_file.open(QIODevice::WriteOnly)){
            //先写头，再写数据，再seek0覆盖头
            dst_file.write((const char *)&head,sizeof(head));

            //缓存pcm数据，达到一定size再写入文件
            QByteArray pcm_temp;
            //数据总大小
            unsigned int size_count=0;
            //toPcm的回调参数
            std::function<bool(const char*,int)> call_back=[&](const char* pcmData,int pcmSize)
            {
                //每次只写一点速度比较慢
                //dst_file.write(pcmData,pcmSize);
                pcm_temp.append(pcmData,pcmSize);
                size_count+=pcmSize;
                //每次写10M
                if(pcm_temp.count()>1024*1024*10){
                    dst_file.write(pcm_temp);
                    pcm_temp.clear();
                }
                return true;
            };

            trans_result=toPcm(srcpath,params,call_back);

            if(trans_result){
                //尾巴上那点写文件
                dst_file.write(pcm_temp);
                EasyAudioContext context{srcpath};
                if(context.isValid()){
                    EasyAudioParameter in_param=context.getAudioParameter();
                    EasyAudioParameter out_param=getOutParameter(in_param,params);
                    head=createWavHead(out_param.sampleRate,out_param.channels,out_param.sampleFormat,size_count);
                    //覆盖头
                    dst_file.seek(0);
                    dst_file.write((const char *)&head,sizeof(head));
                }else{
                    trans_result=false;
                }
            }

            dst_file.close();
        }

        //无效的转换就把那个文件删除
        if(!trans_result){
            dst_file.remove();
        }
    }
    qDebug()<<"transcodeToWav end."<<trans_result;
    return trans_result;
}

bool EasyAudioDecoder::toPcm(const QSharedPointer<EasyAudioContext> &contextPtr,
                             const EasyAudioParameter &params,
                             std::function<bool (const char *, int)> callBack)
{
    //无效的上下文
    if(contextPtr.isNull()||!contextPtr->isValid())
        return false;

    //描述存储压缩数据
    //视频通常包含一个压缩帧，音频可能包含多个压缩帧
    AVPacket *packet = NULL;
    //描述原始数据
    AVFrame *frame = NULL;
    //重采样上下文
    SwrContext *swr_ctx = NULL;
    //解析时out缓冲
    int out_bufsize=1024*1024*2; //默认单个通道1M大小
    uint8_t *out_buffer=new uint8_t[out_bufsize];
    uint8_t *out_buffer_arr[2] = {out_buffer,out_buffer+out_bufsize/2};

    //没有变量来接收的话会立即执行
    auto clean=qScopeGuard([&]{
        if(frame){
            av_frame_unref(frame);
            av_frame_free(&frame);
        }
        if(packet){
            //av_free_packet改用av_packet_unref
            av_packet_unref(packet);
            av_packet_free(&packet);
        }
        if(swr_ctx){
            swr_close(swr_ctx);
            swr_free(&swr_ctx);
        }
        if (out_buffer)
            delete [] out_buffer;
        qDebug()<<"toPcm clean";
    });
    Q_UNUSED(clean);

    packet=av_packet_alloc();
    av_init_packet(packet);
    frame=av_frame_alloc();

    //原数据格式参数
    EasyAudioParameter in_param=contextPtr->getAudioParameter();
    const int in_channels = in_param.channels;
    const AVSampleFormat in_sample_fmt=in_param.sampleFormat;
    const int in_sample_rate=in_param.sampleRate;
    //目标格式的参数（如果传入的参数无效，就保持输入格式）
    EasyAudioParameter out_param=getOutParameter(in_param,params);
    const int out_channels = out_param.channels;
    const AVSampleFormat out_sample_fmt=out_param.sampleFormat;
    const int out_sample_rate=out_param.sampleRate;
    //区分planar和packed
    const bool out_is_planar=(out_sample_fmt>AV_SAMPLE_FMT_DBL&&out_sample_fmt!=AV_SAMPLE_FMT_S64);
    //返回每个sample的字节数，S16=2 bytes
    const int sample_bytes=av_get_bytes_per_sample(out_sample_fmt);

    //分配SwrContext并设置/重置公共参数
    //返回NULL为失败，否则分配上下文
    //（目前的需求默认是转为单声道 16bit 的，只有采样率会设置）
    swr_ctx=swr_alloc_set_opts(NULL, //现有的swr上下文，不可用则为NULL
                               av_get_default_channel_layout(out_channels), //输出通道布局 (AV_CH_LAYOUT_*)
                               out_sample_fmt, //输出采样格式 (AV_SAMPLE_FMT_*).
                               out_sample_rate, //输出采样频率 (frequency in Hz)
                               av_get_default_channel_layout(in_channels), //输入通道布局 (AV_CH_LAYOUT_*)
                               in_sample_fmt, //输入采样格式 (AV_SAMPLE_FMT_*).
                               in_sample_rate, //输入采样频率 (frequency in Hz)
                               0, NULL); //日志相关略
    if(swr_ctx==NULL)
        return false;

    //初始化
    //如果要修改转换的参数，调用参数设置后再次init
    if(swr_init(swr_ctx)<0)
        return false;

    int ret=0;

    //因为av_read_frame后就到了下一帧，为了重入先seek到起始处
    //参数一: 上下文;
    //参数二: 流索引, 如果stream_index是-1，会选择一个默认流，时间戳会从以AV_TIME_BASE为单位向具体流的时间基自动转换。
    //参数三: 将要定位处的时间戳，time_base单位或者如果没有流是指定的就用av_time_base单位。
    //参数四: seek功能flag；
    //AVSEEK_FLAG_BACKWARD  是seek到请求的timestamp之前最近的关键帧
    //AVSEEK_FLAG_BYTE 是基于字节位置的查找
    //AVSEEK_FLAG_ANY 是可以seek到任意帧，注意不一定是关键帧，因此使用时可能会导致花屏
    //AVSEEK_FLAG_FRAME 是基于帧数量快进
    //返回值：成功返回>=0
    if(av_seek_frame(contextPtr->formatCtx,-1,0,AVSEEK_FLAG_ANY)<0)
        return false;

    //av_read_frame取流的下一帧，这里循环读取
    //返回0表示成功，小于0表示错误或者文件尾
    while (av_read_frame(contextPtr->formatCtx, packet)>=0)
    {
        //取音频
        if (packet->stream_index == contextPtr->streamIndex)
        {
            //提供原始数据作为解码器的输入(将packet写入到解码队列当中去)
            //返回0表示成功
            ret=avcodec_send_packet(contextPtr->codecCtx, packet);
            if(ret!=0)
                continue;

            //从解码器循环取数据帧
            //返回0表示成功
            while (avcodec_receive_frame(contextPtr->codecCtx, frame)==0)
            {
                //下一个采样数的上限
                //swr_get_out_samples貌似算出来的比av_rescale_rnd多一丢丢
                //但是比最终导出的采样数多一点
                const int out_samples=swr_get_out_samples(swr_ctx,frame->nb_samples);
                //const int out_samples=av_rescale_rnd(swr_get_delay(swr_ctx, in_sample_rate)+
                //                                        frame->nb_samples,
                //                                        out_sample_rate,
                //                                        contextPtr->codecCtx->sample_rate,
                //                                        AV_ROUND_ZERO);
                //qDebug()<<out_samples<<out_bufsize<<sample_bytes*out_samples*out_channels;
                //缓冲区大小是否足够，不够就根据计算值扩充缓冲区大小，且比实际值大0.5倍
                if(out_bufsize<sample_bytes*out_samples*out_channels){
                    delete[] out_buffer;
                    out_bufsize=sample_bytes*out_samples*out_channels*1.5;
                    out_buffer=new uint8_t[out_bufsize];
                    out_buffer_arr[0]=out_buffer;
                    out_buffer_arr[1]=out_buffer+out_bufsize/2;
                }

                //重采样转换
                //如果传入的nb_samles大于了传出的nb_samplse则SwrContext中会有缓存
                //如果有缓存，swr_get_out_samples第二个参数填零取大小，swr_convert最后两个也填0来获取数据
                //通过使用swr_get_out_samples来获取下一次调用swr_convert在给定输入样本数量下输出样本数量的上限，来提供足够的空间。
                //如果是planar类型比如AV_SAMPLE_FMT_S16P,每个data[0]就是左声道，data[1]就是右声道。
                //如果是packed类型，这种类型左右声道的数据都是一个一维数据连续存放的。LRLRLRLR...，就只有data[0]有数据。
                //return每个通道输出的样本数，出错时为负值
                ret = swr_convert(swr_ctx, out_buffer_arr, out_samples,
                                  (const uint8_t **)frame->data,
                                  frame->nb_samples);

                if (ret <= 0) {
                    av_frame_unref(frame);
                    continue;
                }

                //获取给定音频参数所需的缓冲区大小=通道数 * 采样点数* 采样位数/8
                const int out_bufuse = av_samples_get_buffer_size(NULL, out_channels, ret, out_sample_fmt, 1);
                //qDebug()<<"out"<<out_bufuse<<"sample"<<ret<<"channel"<<out_channels<<sample_bytes*out_samples;
                //2021-04-01更正，之前导出没有处理多声道数据，导致数据长度不一致
                //（因为原本的设计是双声道可以导出为两个单声道文件）
                if(out_bufuse > 0){
                    //回调false则整体失败返回false
                    if(!callBack((const char*)out_buffer, out_bufuse)){
                        return false;
                    }
                }
                //if(out_channels==2)
                //{
                //    //双声道时提取左声道数据
                //    //双声道区分planaer和packed
                //    if(out_is_planar){
                //        //planaer左右声道单独放的
                //        if(!callBack((const char*)out_buffer_arr[0], out_bufuse/2)){
                //            return false;
                //        }
                //    }else{
                //        //packed都在[0]，一左一右存放
                //        for(int i = 0; i < out_bufuse; i += sample_bytes*2)
                //        {
                //            //回调false则整体失败返回false
                //            if(!callBack((const char*)out_buffer_arr[0] + i, sample_bytes)){
                //                return false;
                //            }
                //        }
                //    }
                //}else if(out_channels==1){
                //    //单声道数据
                //    //回调false则整体失败返回false
                //    if(!callBack((const char*)out_buffer_arr[0], out_bufuse)){
                //        return false;
                //    }
                //}
                av_frame_unref(frame);
            }
        }
        av_packet_unref(packet);
    }
    qDebug()<<"toPcm end";
    return true;
}

bool EasyAudioDecoder::toPcm(const QString &filepath, const EasyAudioParameter &params, std::function<bool (const char *, int)> callBack)
{
    return toPcm(QSharedPointer<EasyAudioContext>(new EasyAudioContext(filepath)),params,callBack);
}

EasyWavHead EasyAudioDecoder::createWavHead(int sampleRate, int channels, AVSampleFormat sampleFormat, unsigned int dataSize)
{
    const int bits=av_get_bytes_per_sample(sampleFormat)*8;
    const int head_size = sizeof(EasyWavHead);
    EasyWavHead wav_head;
    memset(&wav_head, 0, head_size);

    memcpy(wav_head.riffFlag, "RIFF", 4);
    memcpy(wav_head.waveFlag, "WAVE", 4);
    memcpy(wav_head.fmtFlag, "fmt ", 4);
    memcpy(wav_head.dataFlag, "data", 4);

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
    //除去头的数据长度
    wav_head.dataSize = dataSize;

    return wav_head;
}

EasyAudioParameter EasyAudioDecoder::getOutParameter(const EasyAudioParameter &inParams, const EasyAudioParameter &outParams)
{
    //如果导出的参数无效，就用输入数据的参数
    EasyAudioParameter param;
    param.channels=(outParams.channels<1)
            ?inParams.channels
           :outParams.channels;
    param.sampleFormat=(outParams.sampleFormat<=AV_SAMPLE_FMT_NONE||outParams.sampleFormat>=AV_SAMPLE_FMT_NB)
            ?inParams.sampleFormat
           :outParams.sampleFormat;
    param.sampleRate=(outParams.sampleRate<1)
            ?inParams.sampleRate
           :outParams.sampleRate;
    return param;
}
