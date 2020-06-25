#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <thread>

#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnStart,&QPushButton::clicked,[this](){
        qDebug()<<"点击 start"<<QThread::currentThread();
        if(!myWatcher.isFinished()){
            qDebug()<<"老哥，当前任务还没结束，等等呗";
            return;
        }
        QFutureInterface<bool> interface;
        interface.reportStarted();
        interface.setProgressRange(0,100);
        myWatcher.setFuture(interface.future());
        //QFutureInterface是允许拷贝的，里面有一个d指针
        std::thread thread([this,interface]() mutable {
            qDebug()<<"线程开始"<<QThread::currentThread();
            //使用sleep模拟耗时处理过程
            bool result=true;
            for(int i=0;i<10;i++)
            {
                //操作前判断是否结束
                if(interface.isPaused()){
                    //pause后不接收值了
                    //interface.setProgressValueAndText(0,"Cancel");
                    result=false;
                    //不恢复watcher不能收到消息
                    interface.setPaused(false);
                    break;
                }

                //使用sleep模拟耗时处理过程
                QThread::msleep(200);

                //设置值前判断是否结束
                if(interface.isPaused()){
                    result=false;
                    interface.setPaused(false);
                    break;
                }
                const int progress=(i+1)*10;
                interface.setProgressValueAndText(
                            progress,
                            QString("已处理 %%1").arg(progress));
            }
            interface.reportResult(result);
            interface.reportFinished();
            qDebug()<<"线程结束"<<QThread::currentThread();
        });
        thread.detach(); //分离不管他了，用future来确保退出
    });
    connect(ui->btnCancel,&QPushButton::clicked,[this](){
        qDebug()<<"点击 cancel"<<QThread::currentThread();
        //因为调用cancel会异常，所以我先拿pause来实现结束标志位的设置，会在线程中判断该标志
        //线程中的分段任务完成才会退出
        myWatcher.pause();
    });

    //异步结果
    connect(&myWatcher,&QFutureWatcher<bool>::finished,[=]{
        qDebug()<<"任务已完成"<<myWatcher.result();
        if(myWatcher.result()){
            ui->label->setText("任务完成");
        }else{
            ui->label->setText("任务失败");
        }
    });
    connect(&myWatcher,&QFutureWatcher<bool>::paused,[=]{
        qDebug()<<"任务已取消";
    });

    //进度条相关
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);
    connect(&myWatcher,&QFutureWatcher<bool>::progressValueChanged,[=](int value){
        qDebug()<<"Value"<<value<<QThread::currentThread();
        ui->progressBar->setValue(value);
    });
    connect(&myWatcher,&QFutureWatcher<bool>::progressTextChanged,[=](const QString text){
        qDebug()<<"Text"<<text<<QThread::currentThread();
        ui->label->setText(text);
    });
}

MainWindow::~MainWindow()
{
    myWatcher.pause();
    myWatcher.waitForFinished();
    delete ui;
}

