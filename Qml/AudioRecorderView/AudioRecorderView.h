#ifndef AUDIORECORDERVIEW_H
#define AUDIORECORDERVIEW_H

#include <QQuickPaintedItem>
#include <QAbstractNativeEventFilter>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "AudioRecorderDevice.h"
#include "AudioRecorderInput.h"
#include "AudioRecorderOutput.h"

/**
 * @brief 录音可视化组件
 * @author 龚建波
 * @date 2020-12-4
 * @details
 * 录制可参照示例：audio和audioinput
 * 在操作录制、播放时会先调用stop，重置状态
 *
 * @history
 * 2020-12-11 增加saveToCache接口
 * （导入音频库时为了文件名唯一性，目前采用uuid作为文件名，所以保存的时候名字用uuid）
 * （显示的名字映射该uuid）
 * 2020-12-14 增加设备插拔监测
 */
class AudioRecorderView : public QQuickPaintedItem
        , public QAbstractNativeEventFilter
        , public AudioRecorderBase
{
    Q_OBJECT
    Q_PROPERTY(AudioRecorderView::RecordState recordState READ getRecordState NOTIFY recordStateChanged)
    Q_PROPERTY(AudioRecorderView::ViewMode viewMode READ getViewMode WRITE setViewMode NOTIFY viewModeChanged)
    Q_PROPERTY(AudioRecorderInput *input READ getInput CONSTANT)
    Q_PROPERTY(AudioRecorderOutput *output READ getOutput CONSTANT)
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged)
    Q_PROPERTY(QString durationString READ getDurationString NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ getPosition NOTIFY positionChanged)
    Q_PROPERTY(QString positionString READ getPositionString NOTIFY positionChanged)
    Q_PROPERTY(bool hasData READ getHasData NOTIFY hasDataChanged)
    //目前这些属性不会触发交互，暂时用member
    Q_PROPERTY(int leftPadding MEMBER leftPadding)
    Q_PROPERTY(int rightPadding MEMBER rightPadding)
    Q_PROPERTY(int topPadding MEMBER topPadding)
    Q_PROPERTY(int bottomPadding MEMBER bottomPadding)
    Q_PROPERTY(int radius MEMBER radius)
    Q_PROPERTY(QColor backgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor viewColor MEMBER viewColor)
    Q_PROPERTY(QColor gridColor MEMBER gridColor)
    Q_PROPERTY(QColor seriesColor MEMBER seriesColor)
    Q_PROPERTY(QColor cursorColor MEMBER cursorColor)
    Q_PROPERTY(QColor axisColor MEMBER axisColor)
    Q_PROPERTY(QColor textColor MEMBER textColor)
public:
    //状态
    enum RecordState
    {
        Stop //默认停止状态
        ,Playing //播放
        ,PlayPause //播放暂停
        ,Record //录制
    };
    Q_ENUM(RecordState)
    //显示模式
    enum ViewMode
    {
        FullRange //绘制全部数据
        ,Tracking //跟踪最新数据
    };
    Q_ENUM(ViewMode)
public:
    explicit AudioRecorderView(QQuickItem *parent = nullptr);
    ~AudioRecorderView();

    //录制状态
    AudioRecorderView::RecordState getRecordState() const { return recordState; }
    void setRecordState(AudioRecorderView::RecordState state);

    //绘制模式
    AudioRecorderView::ViewMode getViewMode() const { return viewMode; }
    void setViewMode(AudioRecorderView::ViewMode mode);

    //输入
    AudioRecorderInput* getInput() { return &audioInput; }
    //输出
    AudioRecorderOutput* getOutput() { return &audioOutput; }

    //当前数据的总时长ms
    qint64 getDuration() const { return audioDuration; }
    void setDuration(qint64 duration);
    //将duration毫秒数转为时分秒格式
    QString getDurationString() const;

    //当前播放或者录制的时间ms
    qint64 getPosition() const { return audioPostion; }
    void updatePosition();
    QString getPositionString() const;

    //当前是否有数据
    bool getHasData() const;
    void setHasData(bool has);

    //获取到的录音数据
    qint64 writeData(const char *data, qint64 maxSize) override;
    //导出缓存数据
    qint64 readData(char *data, qint64 maxSize) override;

    //停止操作
    Q_INVOKABLE void stop();
    //播放
    //device:输入设备名称
    Q_INVOKABLE void play(const QString &device=QString());
    //暂停播放
    Q_INVOKABLE void suspendPlay();
    //暂停后恢复
    Q_INVOKABLE void resumePlay();
    //开始录音
    //sampleRate:输入采样率
    //device:输入设备名称
    Q_INVOKABLE void record(int sampleRate,const QString &device=QString());

    //从文件读取
    //暂时不带解析器，只能解析44字节固定wav-pcm
    //（即本组件生成的wav文件）
    Q_INVOKABLE bool loadFromFile(const QString &filepath);

    //保存到文件
    Q_INVOKABLE bool saveToFile(const QString &filepath);
    //保存到cache路径
    //（因为导入到音频库是以uuid为文件名，所以传入的文件名为uuid）
    //return 完整路径，路径为空则保存失败
    Q_INVOKABLE QString saveToCache(const QString &uuid);

    //刷新，调用update
    Q_INVOKABLE void refresh();

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

    //去掉padding的宽高
    int plotAreaWidth() const;
    int plotAreaHeight() const;
    //更新数据点抽样，绘制时根据抽样绘制
    void updateDataSample();
    //计算y周像素间隔
    void calculateSpace(double yAxisLen);
    double calculateSpaceHelper(double valueRefRange, int dividend) const;

signals:
    void recordStateChanged();
    void viewModeChanged();
    void durationChanged();
    void positionChanged();
    void hasDataChanged();
    //输入输出设备变更，state为change时view的状态
    void inputDeviceChanged(AudioRecorderView::RecordState oldState);
    void outputDeviceChanged(AudioRecorderView::RecordState oldState);

private:
    //QAudioInput/Output处理数据时回调IODevice的接口
    AudioRecorderDevice *audioIODevice=nullptr;
    //处理输入
    AudioRecorderInput audioInput;
    //处理输出
    AudioRecorderOutput audioOutput;
    //输出数据计数
    qint64 outputCount=0;
    //播放数据计数
    qint64 playCount=0;

    //当前状态
    RecordState recordState=Stop;
    //绘制模式
    ViewMode viewMode=Tracking;
    //数据缓冲区
    QByteArray audioData;
    bool hasData=false;
    //表示一个绘制用的抽样点信息
    struct SamplePoint
    {
        //目前是没有滚轮缩放的
        //暂时抽样时就把像素位置算好了
        int x; //像素坐标，相对于横轴0点
        int y; //像素坐标，相对于横轴0点
    };
    //绘制的抽样数据
    QList<SamplePoint> sampleData;
    //刷新计时器,elapsed返回毫秒
    QElapsedTimer updateElapse;
    //刷新定时器
    QTimer updateTimer;
    //数据时长ms
    qint64 audioDuration=0;
    //播放或者录制时长ms
    qint64 audioPostion=0;

    //四个边距
    //该版本刻度是一体的，所以刻度的宽高也算在padding里
    int leftPadding=60;
    int rightPadding=5;
    int topPadding=5;
    int bottomPadding=5;
    //圆角
    int radius=0;

    //颜色
    //背景色
    QColor backgroundColor{34,34,34};
    //图区域颜色
    QColor viewColor{10,10,10};
    //网格颜色
    QColor gridColor{50,50,50};
    //曲线颜色
    QColor seriesColor{30,144,255};
    //游标颜色
    QColor cursorColor{200,10,10};
    //刻度轴颜色
    QColor axisColor{200,200,200};
    //文本颜色
    QColor textColor{200,200,200};

    //计算刻度间隔
    double y1PxToValue=1;
    double y1ValueToPx=1;
    double yRefPxSpace=40;
    int yValueSpace=1000;
};

#endif // AUDIORECORDERVIEW_H
