#ifndef MYWORKER_H
#define MYWORKER_H

#include <QObject>
#include <QTimer>

//继承QObject，然后使用moveToThread函数
class MyWorker : public QObject
{
    //还要加上Q_OBJECT宏才能支持信号槽
    Q_OBJECT
public:
    explicit MyWorker(QObject *parent = nullptr);

signals:
    //通过信号将数据传递给ui线程的槽函数
    void signalMessage(const QString &str);

public slots:
    //通过信号来触发子线程槽函数
    void slotDoing(const QString &str);
    //定时器开关
    void slotTimerSwitch(bool start);

private:
    //定时器用来执行定时任务
    QTimer *timer;
};

#endif // MYWORKER_H
