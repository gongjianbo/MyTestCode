#include "ClientWindow.h"
#include "ui_ClientWindow.h"
#include <QDebug>

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    //连接服务器
    connect(ui->btnConnect,&QPushButton::clicked,[this]{
        if(remoteNode)
            return;
        qDebug()<<"new node";
        remoteNode = new QRemoteObjectNode(this);
        //remoteNode->connectToNode(QUrl("tcp://127.0.0.1:12315"));
        remoteNode->connectToNode(QUrl("local:qro_test"));
        qDebug()<<"connectToNode finished";
        InterfaceReplica *irep = remoteNode->acquire<InterfaceReplica>();
        connect(irep,&InterfaceReplica::stateChanged,
                this,[this](QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState){
            //五种状态，详见文档
            //Uninitialized，动态初始值
            //Default，静态初始值
            //Valid，已连接
            //Suspect，连接后丢失
            //SignatureMismatch，已建立连接，但不是一个rep派生的
            qDebug()<<"replica state changed"<<state<<oldState;
            ui->editRecv->append("state changed:"+QString::number(state));
        });
        //连接接口的信号槽
        connect(irep,&InterfaceReplica::dataChanged,[=](const QString &data){
            //测试直接调用槽函数获取返回值
            auto ret = irep->getData();
            ret.waitForFinished(1000);
            if(ret.error() == QRemoteObjectPendingCall::NoError){
                ui->editRecv->append(ret.returnValue());
            }
        });
        replica.reset(irep);
        //可以等待连接，如果服务器没脸上，会在wait这里阻塞
        //如果服务可用，可以用isReplicaValid判断
        //实测，服务器重启后会自动连接，即使waitForSource的时候没连上
        //irep->waitForSource(5000);
        //qDebug()<<"waitForSource finished";
    });

    //断开连接
    connect(ui->btnClose,&QPushButton::clicked,[this]{
        if(!remoteNode)
            return;
        remoteNode->deleteLater();
        remoteNode = nullptr;
        replica.clear();
    });

    //发送
    connect(ui->btnSend,&QPushButton::clicked,[this]{
        if(!remoteNode)
            return;
        qDebug()<<"send"<<replica->isReplicaValid()<<ui->editSend->text();
        if(!replica->isReplicaValid())
            return;
        replica->setData("client setData:"+ui->editSend->text());
    });
}

ClientWindow::~ClientWindow()
{
    delete ui;
    replica.clear();
}

