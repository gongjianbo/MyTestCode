#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QThread>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //关联future结束信号，注意watcher一次只能绑定一个future
    connect(&theWatcher,&QFutureWatcher<QString>::finished,this,[this]{
        qDebug()<<"Future finished"<<QThread::currentThread();
        //结束之后从future获取结果
        qDebug()<<"Future result"<<theWatcher.future().result();
    });

    //点击按钮从concurrent生成一个future
    connect(ui->btnA,&QPushButton::clicked,[this]{
        qDebug()<<"A click"<<QThread::currentThread();
        //可以传递参数给线程处理
        QFuture<QString> future=QtConcurrent::run([](const QString &arg){
            qDebug()<<"A begin"<<QThread::currentThread();
            QThread::msleep(1000);
            return QString("This is A result.%1").arg(arg);
        },
        QString("Gong Jian Bo."));
        theWatcher.setFuture(future);
    });

    //点击按钮从future interface生成一个future
    connect(ui->btnB,&QPushButton::clicked,[this]{
        qDebug()<<"B click"<<QThread::currentThread();

        QtConcurrent::run([this]{
            theInterface.reportStarted();
            qDebug()<<"B begin"<<QThread::currentThread();
            QThread::msleep(1000);
            theInterface.reportResult(QString("This is B result"));
            theInterface.reportFinished();
        });

        //可以把interface和线程封装到一个类中，如配合QRunnable字类
        theWatcher.setFuture(theInterface.future());
    });
}

MainWindow::~MainWindow()
{
    theWatcher.waitForFinished();
    delete ui;
}

