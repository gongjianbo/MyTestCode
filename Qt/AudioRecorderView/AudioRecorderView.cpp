#include "AudioRecorderView.h"

#include <cmath>

#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QDebug>

AudioRecorderView::AudioRecorderView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //作为QAudioInput/Output的构造参数，处理时回调writeData接口
    audioIODevice=new AudioRecorderDevice(this,this);
    audioIODevice->open(QIODevice::ReadWrite);

    //刷新延时
    updateTimer.setSingleShot(true);
    connect(&updateTimer,&QTimer::timeout,this,[this]{
        update();
    });

    //抽样点绘制
    sampleData.reserve(10000); //预置元素内存

    //输入输出状态
    connect(&audioInput,&AudioRecorderInput::stateChanged,
            this,[this](QAudio::State state){
        //qDebug()<<"input state"<<state;
    });
    connect(&audioOutput,&AudioRecorderOutput::stateChanged,
            this,[this](QAudio::State state){
        //qDebug()<<"output state"<<state;
    });
}

AudioRecorderView::~AudioRecorderView()
{
    stop();
    audioIODevice->close();
}

void AudioRecorderView::setRecordState(AudioRecorderView::RecordState state)
{
    if(recordState!=state)
    {
        recordState=state;
        switch (recordState)
        {
        case Stop: break;
        case Playing: break;
            //case Pause: break;
        case Record: break;
        default:
            break;
        }
        emit recordStateChanged();
    }
}

void AudioRecorderView::setViewMode(AudioRecorderView::ViewMode mode)
{
    if(viewMode!=mode)
    {
        viewMode=mode;
        //TODO 清空数据
        refresh();
    }
}

qint64 AudioRecorderView::writeData(const char *data, qint64 maxSize)
{
    //默认为单声道，16bit
    audioData.append(data,maxSize);
    updateDataSample();
    refresh();
    return maxSize;
}

qint64 AudioRecorderView::readData(char *data, qint64 maxSize)
{
    if(!data||maxSize<1)
        return 0;
    const int data_size=audioData.count()-outputPos;
    if(data_size<=0){
        //stateChanged没有触发停止，懒得判断notify了
        QTimer::singleShot(0,[this]{ stop(); });
        return 0;
    }

    const int read_size=(data_size>=maxSize)?maxSize:data_size;
    memcpy(data,audioData.constData()+outputPos,read_size);
    outputPos+=read_size;
    return read_size;
}

void AudioRecorderView::stop()
{
    //录制、播放时都会调用stop，所以把一些状态重置放这里
    outputPos=0;
    switch (recordState)
    {
    case Stop: break;
    case Playing:
        audioOutput.stopPlay();
        break;
        //case Pause: break;
    case Record:
        audioInput.stopRecord();
        break;
    default:
        break;
    }
    setRecordState(Stop);
}

void AudioRecorderView::play(const QString &device)
{
    stop();

    if(audioData.isEmpty())
        return;
    QAudioFormat format=audioInput.inputFormat;
    if(audioOutput.startPlay(audioIODevice,format,device)){
        //切换为播放状态
        setRecordState(Playing);
    }else{
        qDebug()<<"播放失败";
    }
}

void AudioRecorderView::record(int sampleRate, const QString &device)
{
    stop();
    //录制时清空数据缓存
    audioData.clear();
    sampleData.clear();

    QAudioFormat format;
    format.setSampleRate(sampleRate);
    if(audioInput.startRecord(audioIODevice,format,device)){
        //切换为录制状态
        setRecordState(Record);
    }else{
        qDebug()<<"录制失败";
    }
}

bool AudioRecorderView::loadFromFile(const QString &filepath)
{
    stop();
    return audioInput.loadFromFile(audioIODevice,filepath);
}

bool AudioRecorderView::saveToFile(const QString &filepath)
{
    stop();
    return audioOutput.saveToFile(audioData,audioInput.inputFormat,filepath);
}

void AudioRecorderView::refresh()
{
    //大于xx ms立即刷新，否则定时器刷新
    if(updateElapse.elapsed()>50){
        updateTimer.stop();
        update();
        updateElapse.restart();
    }else{
        //未结束则重新start
        updateTimer.start(50);
    }
}

void AudioRecorderView::paint(QPainter *painter)
{
    painter->fillRect(0,0,width(),height(),QColor(20,50,70));

    //绘制波形
    painter->translate(0,height()/2);
    painter->setPen(QColor(100,150,80));
    //qDebug()<<sampleData.count();
    for(int i=0;i<sampleData.count()-1;i++)
    {
        const SamplePoint &cur=sampleData.at(i);
        const SamplePoint &next=sampleData.at(i+1);
        painter->drawLine(cur.x,cur.y,next.x,next.y);
    }
}

void AudioRecorderView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry,oldGeometry);
    updateDataSample();
    refresh();
}

void AudioRecorderView::updateDataSample()
{
    sampleData.clear();

    const int data_count=audioData.count();
    if(data_count<2&&data_count%2!=0)
        return;
    const int v_padding=10;
    const int sample_count=data_count/2;
    const short *data_ptr=(const short*)audioData.constData();
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step=std::ceil(sample_count/(double)width())/2;
    if(x_step<1)
        x_step=1;
    else if(x_step>sample_count)
        x_step=sample_count;
    //坐标轴轴适应
    const double x_scale=width()/(double)sample_count;
    const double y_scale=-(height()-v_padding*2)/(double)0x10000;

    short cur_max=0;
    short cur_min=0;
    int index_max=0;
    int index_min=0;
    //分段找最大最小作为该段的抽样点
    for(int i=0;i<sample_count;i+=x_step)
    {
        cur_max=data_ptr[i];
        cur_min=data_ptr[i];
        index_max=i;
        index_min=i;
        for(int j=i;j<i+x_step&&j<sample_count;j++)
        {
            //遍历找这一段的最大最小值
            if(cur_max<data_ptr[j]){
                cur_max=data_ptr[j];
                index_max=j;
            }
            if(cur_min>data_ptr[j]){
                cur_min=data_ptr[j];
                index_min=j;
            }
        }
        SamplePoint pt_min{ int(index_min*x_scale),int(cur_min*y_scale) };
        SamplePoint pt_max{ int(index_max*x_scale),int(cur_max*y_scale) };
        //根据先后顺序存最大最小，相等就存一个
        if(index_max>index_min){
            sampleData<<pt_min<<pt_max;
        }else if(index_max<index_min){
            sampleData<<pt_max<<pt_min;
        }else{
            sampleData<<pt_min;
        }
    }
}
