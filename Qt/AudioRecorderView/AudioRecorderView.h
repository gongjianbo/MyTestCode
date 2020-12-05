#ifndef AUDIORECORDERVIEW_H
#define AUDIORECORDERVIEW_H

#include <QQuickPaintedItem>
#include <QQmlParserStatus>
#include <QTime>
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
 * @note
 * 获取输入设备名重复是插件问题，在新版本可以判断插件（如5.15）
 * 参见：（Qt4）QTBUG-16841（Qt5）QTBUG-75781
 * 暂时用采样率来过滤输入设备
 */
class AudioRecorderView : public QQuickPaintedItem, public AudioRecorderBase
{
    Q_OBJECT
    Q_PROPERTY(AudioRecorderView::RecordState recordState READ getRecordState WRITE setRecordState NOTIFY recordStateChanged)
    Q_PROPERTY(AudioRecorderView::ViewMode viewMode READ getViewMode WRITE setViewMode NOTIFY viewModeChanged)
    Q_PROPERTY(AudioRecorderInput *input READ getInput CONSTANT)
    Q_PROPERTY(AudioRecorderOutput *output READ getOutput CONSTANT)
public:
    //状态
    enum RecordState
    {
        Stop //默认停止状态
        ,Playing //播放
        //,Pause //播放暂停
        ,Record //录制
    };
    Q_ENUM(RecordState)
    //显示模式
    enum ViewMode
    {
        FullRange //绘制全部数据
        ,Tracking //跟踪最新数据
        ,Wiper //雨刷，一屏满了再刷新
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

    //获取到的录音数据
    qint64 writeData(const char *data, qint64 maxSize) override;
    //导出缓存数据
    qint64 readData(char *data, qint64 maxSize) override;

    //停止操作
    Q_INVOKABLE void stop();

    //播放
    //device:输入设备名称
    Q_INVOKABLE void play(const QString &device=QString());

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

    //刷新，调用update
    Q_INVOKABLE void refresh();

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    //更新数据点抽样，绘制时根据抽样绘制
    void updateDataSample();

signals:
    void recordStateChanged();
    void viewModeChanged();

private:
    //QAudioInput/Output处理数据时回调IODevice的接口
    AudioRecorderDevice *audioIODevice=nullptr;
    //处理输入
    AudioRecorderInput audioInput;
    //处理输出
    AudioRecorderOutput audioOutput;
    //输出数据计数
    qint64 outputPos=0;

    //当前状态
    RecordState recordState=Stop;
    //绘制模式
    ViewMode viewMode=Tracking;
    //数据缓冲区
    QByteArray audioData;
    //表示一个绘制用的抽样点信息
    struct SamplePoint
    {
        int x; //像素坐标，相对于横轴0点
        int y; //像素坐标，相对于横轴0点
    };
    //绘制的抽样数据
    QList<SamplePoint> sampleData;
    //刷新计时器,elapsed返回毫秒
    QElapsedTimer updateElapse;
    //刷新定时器
    QTimer updateTimer;
};

#endif // AUDIORECORDERVIEW_H
