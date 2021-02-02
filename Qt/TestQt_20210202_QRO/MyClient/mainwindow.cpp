#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    node=new QRemoteObjectNode(this);
    if(!node->connectToNode(QUrl("local:interfaces"))){
        qDebug()<<"connect error";
    }
    rep = node->acquire<MyRepReplica>();
    connect(rep,&MyRepReplica::mySend,
            this,[=](const QString & data, int num){
        qDebug()<<"recv"<<data<<num;
    });
    connect(ui->pushButton,&QPushButton::clicked,this,[=]{
        if(rep){
            qDebug()<<"send data";
            rep->myRecv("gongjianbo",2021);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

