#include "AudioRecorderView.h"

#include <cmath>
#include <Windows.h>
//#include <Dbt.h>

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
        qDebug()<<"input state"<<state;
    });
    connect(&audioOutput,&AudioRecorderOutput::stateChanged,
            this,[this](QAudio::State state){
        qDebug()<<"output state"<<state;
    });
    //更新播放进度游标
    connect(&audioOutput,&AudioRecorderOutput::notify,this,[this]{
        if(audioOutput.audioOutput&&audioDuration){
            //用processedUSecs获取start到当前的us数，但是start后有点延迟
            //进度=已放时间和总时间之比*总字节数，注意时间单位
            //-50是为了补偿时差，音画同步，这个50就是output的NotifyInterval
            //（还有点问题就是快结束的时候尾巴上那点直接结束了，数据少的时候明显点）
            playCount = (audioOutput.audioOutput->processedUSecs()/1000.0-50)/audioDuration*audioData.count();
            if(playCount>outputCount)
                playCount=outputCount;
            refresh();
            updatePosition();
        }
    });

    //不可见后stop
    connect(this,&AudioRecorderView::visibleChanged,this,[this]{
        if(!isVisible()){
            stop();
        }
    });

    //注册到qApp过滤native事件
    qApp->installNativeEventFilter(this);
}

AudioRecorderView::~AudioRecorderView()
{
    //貌似析构的时候自动会remove
    qApp->removeNativeEventFilter(this);
    stop();
    audioIODevice->close();
}

void AudioRecorderView::setRecordState(AudioRecorderView::RecordState state)
{
    if(recordState!=state)
    {
        const RecordState old_state=getRecordState();
        recordState=state;
        //结束记录后展示全谱
        if(old_state==Record){
            updateDataSample();
        }
        //感觉这里少了点什么
        emit recordStateChanged();
        refresh();
    }
}

void AudioRecorderView::setViewMode(AudioRecorderView::ViewMode mode)
{
    if(viewMode!=mode)
    {
        viewMode=mode;
        refresh();
    }
}

void AudioRecorderView::setDuration(qint64 duration)
{
    if(audioDuration!=duration){
        audioDuration=duration;
        emit durationChanged();
    }
}

QString AudioRecorderView::getDurationString() const
{
    return QTime(0,0).addMSecs(audioDuration).toString("hh:mm:ss");
}

void AudioRecorderView::updatePosition()
{
    if(getRecordState()==Playing||getRecordState()==PlayPause){
        const int sample_rate=audioInput.inputFormat.sampleRate();
        audioPostion=((playCount/2)/(1.0*sample_rate)*1000);
    }else{
        //未播放时positon为0
        audioPostion=0; //getDuration();
    }
    emit positionChanged();
}

QString AudioRecorderView::getPositionString() const
{
    return QTime(0,0).addMSecs(audioPostion).toString("hh:mm:ss");
}

bool AudioRecorderView::getHasData() const
{
    return hasData;
}

void AudioRecorderView::setHasData(bool has)
{
    if(hasData!=has){
        hasData=has;
        emit hasDataChanged();
    }
}

qint64 AudioRecorderView::writeData(const char *data, qint64 maxSize)
{
    //默认为单声道，16bit
    audioData.append(data,maxSize);
    setHasData(!audioData.isEmpty());
    updateDataSample();
    refresh();
    updatePosition();
    return maxSize;
}

qint64 AudioRecorderView::readData(char *data, qint64 maxSize)
{
    if(!data||maxSize<1)
        return 0;
    const int data_size=audioData.count()-outputCount;
    if(data_size<=0){
        //stateChanged没有触发停止，懒得判断notify了
        QTimer::singleShot(0,[this]{ stop(); });
        return 0;
    }

    const int read_size=(data_size>=maxSize)?maxSize:data_size;
    memcpy(data,audioData.constData()+outputCount,read_size);
    outputCount+=read_size;
    //refresh(); 这个间隔太大了
    return read_size;
}

void AudioRecorderView::stop()
{
    //录制、播放时都会调用stop，所以把一些状态重置放这里
    //(停止的时候audioData的数据保留，在start时才清空)
    outputCount=0;
    playCount=0;
    switch (getRecordState())
    {
    case Stop: break;
    case Playing:
    case PlayPause:
        audioOutput.stopPlay();
        break;
    case Record:
        audioInput.stopRecord();
        break;
    default:
        break;
    }
    setRecordState(Stop);
    updatePosition();
}

void AudioRecorderView::play(const QString &device)
{
    //暂停继续
    if(getRecordState()==PlayPause){
        resumePlay();
        return;
    }
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

void AudioRecorderView::suspendPlay()
{
    if(getRecordState()!=Playing)
        return;
    audioOutput.suspendPlay();
    setRecordState(PlayPause);
}

void AudioRecorderView::resumePlay()
{
    if(getRecordState()!=PlayPause)
        return;
    audioOutput.resumePlay();
    setRecordState(Playing);
}

void AudioRecorderView::record(int sampleRate, const QString &device)
{
    stop();
    //录制时清空数据缓存
    audioData.clear();
    setHasData(false);
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
    //加载时清空数据缓存
    audioData.clear();
    setHasData(false);
    sampleData.clear();
    return audioInput.loadFromFile(audioIODevice,filepath);
}

bool AudioRecorderView::saveToFile(const QString &filepath)
{
    stop();
    return audioOutput.saveToFile(audioData,audioInput.inputFormat,filepath);
}

QString AudioRecorderView::saveToCache(const QString &uuid)
{
    stop();
    return audioOutput.saveToCache(audioData,audioInput.inputFormat,uuid);
}

void AudioRecorderView::refresh()
{
    //更新时长信息
    const int sample_rate=audioInput.inputFormat.sampleRate();
    if(sample_rate>0){
        //时长=采样总数/每秒的采样数
        //s time*1000=ms time
        setDuration((audioData.size()/2)/(1.0*sample_rate)*1000);
    }else{
        setDuration(0);
    }

    //大于xx ms立即刷新，否则定时器刷新
    if(updateElapse.elapsed()>30){
        updateTimer.stop();
        update();
        updateElapse.restart();
    }else{
        //未结束则重新start
        updateTimer.start(30);
    }
}

void AudioRecorderView::paint(QPainter *painter)
{
    //暂时没有封装步骤
    //series区域的宽高
    const int view_width=plotAreaWidth();
    const int view_height=plotAreaHeight();
    //series零点坐标
    const int wave_x=leftPadding;
    const int wave_y=view_height/2+topPadding;

    //背景色
    painter->setPen(Qt::NoPen);
    //painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setBrush(backgroundColor);
    painter->drawRoundedRect(0,0,width(),height(),radius,radius);
    //painter->setRenderHint(QPainter::Antialiasing,false);
    painter->setBrush(Qt::NoBrush);
    painter->fillRect(leftPadding,rightPadding,view_width,view_height,viewColor);

    //网格
    painter->translate(wave_x,wave_y);
    painter->setPen(cursorColor);
    painter->drawLine(0,0,view_width,0);
    int y_px=0;
    painter->setPen(gridColor);
    for(int i=yValueSpace;i<0xFFFF/2;i+=yValueSpace)
    {
        y_px=i*y1ValueToPx;
        painter->drawLine(0,y_px,view_width,y_px);
        painter->drawLine(0,-y_px,view_width,-y_px);
    }
    painter->translate(-wave_x,-wave_y);

    //有数据时才绘制曲线
    if(!audioData.isEmpty())
    {
        //绘制波形
        painter->setPen(seriesColor);
        painter->translate(wave_x,wave_y);
        for(int i=0;i<sampleData.count()-1;i++)
        {
            const SamplePoint &cur=sampleData.at(i);
            const SamplePoint &next=sampleData.at(i+1);
            painter->drawLine(cur.x,cur.y,next.x,next.y);
        }
        painter->translate(-wave_x,-wave_y);

        //画游标
        painter->setPen(cursorColor);
        const int play_pos=double(playCount)/audioData.count()*view_width+leftPadding;
        painter->drawLine(play_pos,topPadding,
                          play_pos,height()-bottomPadding);
    }

    //纵轴幅度
    painter->translate(wave_x,wave_y);
    QString y_text;
    painter->setPen(textColor);
    painter->drawText(-5-painter->fontMetrics().width("0"),painter->fontMetrics().height()/2,"0");
    for(int i=yValueSpace;i<0xFFFF/2;i+=yValueSpace)
    {
        y_px=i*y1ValueToPx;
        y_text=QString::number(i);
        painter->drawText(-5-painter->fontMetrics().width(y_text),
                          y_px+painter->fontMetrics().height()/2,
                          y_text);
        y_text=QString::number(-i);
        painter->drawText(-5-painter->fontMetrics().width(y_text),
                          -y_px+painter->fontMetrics().height()/2,
                          y_text);
    }
    painter->translate(-wave_x,-wave_y);
    painter->setPen(axisColor);
    painter->drawLine(leftPadding,topPadding,leftPadding,topPadding+view_height);

    //横轴时间，略
    painter->drawLine(leftPadding,topPadding+view_height,leftPadding+view_width,topPadding+view_height);
}

void AudioRecorderView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry,oldGeometry);
    updateDataSample();
    calculateSpace(plotAreaHeight());
    refresh();
}

bool AudioRecorderView::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if(eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        //设备插拔
        if(msg&&msg->message == WM_DEVICECHANGE)
        {
            //同步阻塞会有警告
            //Could not invoke audio interface activation.
            //(因为应用程序正在发送一个输入同步呼叫，所以无法执行传出的呼叫。)
            QTimer::singleShot(0,[this]{
                //一些详细信息需要注册，这里改为change就去查询输入列表是否变更
                //qDebug()<<"change"<<msg->wParam;
                //switch(msg->wParam)
                //{
                //default: break;
                //    //设备插入
                //case DBT_DEVICEARRIVAL:
                //    qDebug()<<"in";
                //    break;
                //    //设备移除
                //case DBT_DEVICEREMOVECOMPLETE:
                //    qDebug()<<"out";
                //    break;
                //}

                //使用中的设备不存在，停止
                const RecordState cur_state=recordState;
                audioInput.updateInputDevices();
                audioOutput.updateOutputDevices();
                if(!audioInput.checkInputExists()){
                    stop();
                    emit inputDeviceChanged(cur_state);
                }
                if(!audioOutput.checkOutputExists()){
                    emit outputDeviceChanged(cur_state);
                }
            });
        }
    }
    return false;
}

int AudioRecorderView::plotAreaWidth() const
{
    return (width()-leftPadding-rightPadding);
}

int AudioRecorderView::plotAreaHeight() const
{
    return (height()-topPadding-bottomPadding);
}

void AudioRecorderView::updateDataSample()
{
    sampleData.clear();

    const int data_count=audioData.count();
    if(data_count<2&&data_count%2!=0)
        return;

    //根据模式切换显示的数据范围，暂时固定值
    //s*channel*sampleRate
    int data_show=data_count/2;
    if(getViewMode()==Tracking&&getRecordState()==Record){
        const int max_show=5*1*audioOutput.outputFormat.sampleRate();
        if(data_show>max_show)
            data_show=max_show;
    }
    if(data_count<data_show*2)
        return;
    const int sample_count=data_show;
    const short *data_ptr=(const short*)audioData.constData()+(data_count/2-data_show);
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step=std::ceil(sample_count/(double)width())/2;
    if(x_step<1)
        x_step=1;
    else if(x_step>sample_count)
        x_step=sample_count;
    //坐标轴轴适应
    const double x_scale=(width()-leftPadding-rightPadding)/(double)sample_count;
    const double y_scale=-(height()-topPadding-bottomPadding)/(double)0x10000;

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

void AudioRecorderView::calculateSpace(double yAxisLen)
{
    //暂时为固定范围
    y1PxToValue=0xFFFF/(yAxisLen);
    y1ValueToPx=(yAxisLen)/0xFFFF;

    //计算间隔
    double space_ref=y1PxToValue*yRefPxSpace;
    if(space_ref<1)
        space_ref=1;
    yValueSpace=std::round(calculateSpaceHelper(space_ref,1));
}

double AudioRecorderView::calculateSpaceHelper(double valueRefRange, int dividend) const
{
    if(valueRefRange>8*dividend){
        //if(dividend>10000*100)return dividend;
        return calculateSpaceHelper(valueRefRange,dividend*10);
    }else if(valueRefRange>4.5*dividend){
        return 5*dividend;
    }else if(valueRefRange>3*dividend){
        return 4*dividend;
    }else if(valueRefRange>1.5*dividend){
        return 2*dividend;
    }else{
        return dividend;
    }
}
