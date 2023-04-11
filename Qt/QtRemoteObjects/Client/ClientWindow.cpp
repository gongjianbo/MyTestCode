#include "ClientWindow.h"
#include "ui_ClientWindow.h"
#include <QDebug>

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    ui->btnClose->setEnabled(false);

    //连接服务器
    connect(ui->btnConnect,&QPushButton::clicked,[this]{
        if(replica)
            return;
        qDebug()<<"new node";
        //remoteNode = new QRemoteObjectNode(this);
        //remoteNode->connectToNode(QUrl("tcp://127.0.0.1:12315"));
        remoteNode.connectToNode(QUrl("local:qro_test"));
        qDebug()<<"connectToNode finished";
        //可以设置acquire的name，和host.enableRemoting的name配对
        InterfaceReplica *irep = remoteNode.acquire<InterfaceReplica>("QRO");
        //如果stateChanged直连槽函数调用接口，Qt5.15 mac上在拿返回值时会卡很久，可以使用QueuedConnection队列链接
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
            //更新按钮状态
            const bool ok = (state == QRemoteObjectReplica::Valid);
            ui->btnConnect->setEnabled(!ok);
            ui->btnClose->setEnabled(ok);
        }, Qt::QueuedConnection);
        //连接接口的信号槽
        connect(irep,&InterfaceReplica::dataChanged,[=](const QString &data){
            //测试直接调用槽函数获取返回值
            //auto ret = irep->getData();
            //ret.waitForFinished(1000);
            //if(ret.error() == QRemoteObjectPendingCall::NoError){
            //    ui->editRecv->append(ret.returnValue());
            //}
            //或者使用信号带的参数
            ui->editRecv->append(data);
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
        if(!replica)
            return;
        replica.clear();
    });

    //测试信号
    //(实测，replica发信号，souce是感知不到的)
    connect(ui->btnSignal,&QPushButton::clicked,[this]{
        if(!replica)
            return;
        qDebug()<<"signal"<<replica->isReplicaValid()<<ui->editSend->text();
        if(!replica->isReplicaValid())
            return;
        emit replica->dataChanged("client dataChanged:"+ui->editSend->text());
    });
    //测试槽
    connect(ui->btnSlot,&QPushButton::clicked,[this]{
        if(!replica)
            return;
        qDebug()<<"slot"<<replica->isReplicaValid()<<ui->editSend->text();
        if(!replica->isReplicaValid())
            return;
        replica->setData("client setData:"+ui->editSend->text());
    });
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

