#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QtConcurrentRun>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //【示例A】通过信号槽
    thread=new QThread(this);
    //要move到别的线程的话不要传递parent参数
    worker=new MyWorker();
    //更改该对象及其子对象的线程关联性。如果对象具有父对象，则无法移动。
    //事件处理将在targetThread中继续。
    worker->moveToThread(thread);
    //官方示例里的释放方式
    connect(thread,&QThread::finished,worker,&QObject::deleteLater);
    //worker的定时器开关
    ui->btnTimer->setCheckable(true); //Checkable就能有两种状态-对应定时器开关
    connect(ui->btnTimer,&QPushButton::clicked,worker,&MyWorker::slotTimerSwitch);
    //worker执行任务
    connect(ui->btnDoA,&QPushButton::clicked,this,[this]{
        emit signalDoing(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")+" main");
    });
    connect(this,&MainWindow::signalDoing,worker,&MyWorker::slotDoing);
    //worker操作结果
    connect(worker,&MyWorker::signalMessage,ui->textEditA,&QTextEdit::append);
    //启动线程
    thread->start();

    //【示例B】通过invokeMethod方法
    //(这里我直接用concurrent模块的run函数)
    qDebug()<<"main thread"<<QThread::currentThread();
    connect(ui->btnDoB,&QPushButton::clicked,this,[this]{
        QtConcurrent::run([=]{
            qDebug()<<"run doing"<<QThread::currentThread();
            //使用QMetaObject::invokeMethod()操作是线程安全的
            QMetaObject::invokeMethod(this,[=]{ //这个this就是传递进来的mainwindow
                qDebug()<<"invoke doing"<<QThread::currentThread();
                ui->textEditB->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")+" invoke finished");
            });
        });
    });

    //【示例C】通过postEvent
    //(需要重写接收者的event()事件处理函数)
    connect(ui->btnDoC,&QPushButton::clicked,this,[this]{
        QtConcurrent::run([=]{
            qDebug()<<"run doing"<<QThread::currentThread();
            //postEvent是非阻塞的，sendEvent是阻塞的，postEvent是线程安全的
            QApplication::postEvent(this,new MyEvent(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")));
        });
    });
}

MainWindow::~MainWindow()
{
    //退出示例A的线程，注意里面若有死循环要提前break
    thread->quit();
    thread->wait();
    delete ui;
}

bool MainWindow::event(QEvent *event)
{
    if(event->type()==MyEvent::eventType){
        MyEvent *my_event=dynamic_cast<MyEvent*>(event);
        if(my_event){
            //通过postevent传递信息
            ui->textEditB->append(my_event->message+" event finished");
        }
    }

    return QMainWindow::event(event);
}

