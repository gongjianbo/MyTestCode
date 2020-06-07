#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QResource>
#include <QFile>
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //为什么不用combobox？因为按钮样式更简单
    //可以在启动时读取目录下皮肤文件作为combobox的选项
    connect(ui->btnWhite,&QPushButton::clicked,this,[=](){
        loadSkin("white.rcc");
    });
    connect(ui->btnBlack,&QPushButton::clicked,this,[=](){
        loadSkin("black.rcc");
    });
    //加载默认样式
    loadSkin("white.rcc");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSkin(const QString &paths)
{
    static QString old_paths;
    if(paths.isEmpty()||paths==old_paths)
        return;

    //先卸载当前的资源
    if(!old_paths.isEmpty()){
        const bool unresult=QResource::unregisterResource(old_paths);
        qDebug()<<"QResource::unregisterResource"<<unresult;
    }
    old_paths=paths;

    //加载资源文件
    const bool result=QResource::registerResource(paths);
    qDebug()<<"QResource::registerResource"<<result;
    if(result){
        //设置样式表
        QFile file(":/style.qss");
        if(file.open(QIODevice::ReadOnly)){
            const QString qss_str=file.readAll();
            qApp->setStyleSheet(qss_str);
        }
    }
}

