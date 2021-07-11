#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydb.h"
#include <thread>
#include <QtConcurrentRun>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //测试一个qt db连接跨线程使用会有什么异常

    connect(ui->btnA,&QPushButton::clicked,this,[]{
        QtConcurrent::run([]{
            qDebug()<<"clicked a"<<QThread::currentThread();
            MyDb::getInstance()->insertData();
        });
    });

    connect(ui->btnB,&QPushButton::clicked,this,[]{
        QtConcurrent::run([]{
            qDebug()<<"clicked b"<<QThread::currentThread();
            MyDb::getInstance()->insertData();
        });
    });

    connect(ui->btnC,&QPushButton::clicked,this,[]{
        QtConcurrent::run([]{
            qDebug()<<"clicked c"<<QThread::currentThread();
            MyDb::getInstance()->selectData();
        });
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

