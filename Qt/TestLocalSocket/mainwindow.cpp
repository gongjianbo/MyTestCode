#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //【】客户端测试
    //启动客户端
    connect(ui->btnStartClient,&QPushButton::clicked,[=]{
        if(client){
            client->nodeFree();
            client->deleteLater();
            client=nullptr;
        }else{
            client=new VoicePrintClient(this);
            client->init();
        }
    });
    //新增文件
    connect(ui->btnAppend,&QPushButton::clicked,[=]{
        client->appendFile(QStringList()<<ui->editPath->text());
    });
    //修改文件
    connect(ui->btnEdit,&QPushButton::clicked,[=]{
        client->editFile(ui->editPath->text());
    });
    //删除文件
    connect(ui->btnRemove,&QPushButton::clicked,[=]{
        client->removeFile(QStringList()<<ui->editPath->text());
    });
    //比对测试
    connect(ui->btnCompare,&QPushButton::clicked,[=]{
        client->compareRun(QStringList()<<ui->editPath->text(),
                           QStringList()<<ui->editPath->text(),123);
    });
    //聚类测试
    connect(ui->btnCluster,&QPushButton::clicked,[=]{
        client->clusterRun(QStringList()<<ui->editPath->text(),123);
    });

    //【】服务端测试
    //启动进程
    connect(ui->btnStartAlgorithm,&QPushButton::clicked,[=]{
        if(host){
            if(source){
                source->nodeFree();
                source->deleteLater();
                source=nullptr;
            }
            host->hostFree();
            host->deleteLater();
            host=nullptr;
        }else{
            host=new LocalHost(this);
            connect(host,&LocalHost::newNode,[this](LocalNodeCore *core){
                if(!core) return;
                source=new VoicePrintSource(this);
                source->nodeInit(core);
            });
            host->hostInit(VoicePrint_Server);
        }
    });
    //比对
    connect(ui->btnCompareProgress,&QPushButton::clicked,[=]{
        if(source){
            source->updateCompareProgress(11);
        }
    });
    connect(ui->btnCompareFinish,&QPushButton::clicked,[=]{
        if(source){
            source->compareResult(true);
        }
    });
    //聚类
    connect(ui->btnClusterProgress,&QPushButton::clicked,[=]{
        if(source){
            source->updateClusterProgress(22);
        }
    });
    connect(ui->btnClusterFinish,&QPushButton::clicked,[=]{
        if(source){
            source->clusterResult(false);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

