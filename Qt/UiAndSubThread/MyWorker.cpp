#include "MyWorker.h"

#include <QThread>
#include <QDateTime>
#include <QDebug>

MyWorker::MyWorker(QObject *parent)
    : QObject(parent),timer(new QTimer(this))
{
    //定时执行任务
    timer->setInterval(1000); //1s触发间隔
    connect(timer,&QTimer::timeout,this,[this]{
        //这里用时间来表示操作
        slotDoing(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")+" timer");
    });
}

void MyWorker::slotDoing(const QString &str)
{
    //执行任务
    qDebug()<<"slot doing"<<QThread::currentThread();
    //操作结果发送给ui线程
    emit signalMessage(str+" finished");
}

void MyWorker::slotTimerSwitch(bool start)
{
    //定时器开关
    start?timer->start():timer->stop();
    qDebug()<<"timer switch"<<timer->isActive();
}
