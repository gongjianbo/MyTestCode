#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    host=new QRemoteObjectHost(this);
    host->setHostUrl(QUrl("local:interfaces"));

    rep=new MyRep(this);
    host->enableRemoting(rep);
    connect(rep,&MyRep::mySend,this,[=](const QString & data, int num){
        qDebug()<<"send"<<data<<num;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

