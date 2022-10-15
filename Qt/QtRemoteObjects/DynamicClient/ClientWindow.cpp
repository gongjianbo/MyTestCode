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
        QRemoteObjectDynamicReplica *irep = remoteNode.acquireDynamic("QRO");
        connect(irep,&QRemoteObjectDynamicReplica::stateChanged,
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
        });
        //replica初始化完成后再进行信号槽绑定
        connect(irep,&QRemoteObjectDynamicReplica::initialized,
                this,&ClientWindow::initConnection);
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

    //测试调用服务端接口
    connect(ui->btnCall,&QPushButton::clicked,[this]{
        if(!replica)
            return;
        qDebug()<<"dynamic call"<<replica->isReplicaValid()<<ui->editSend->text();
        if(!replica->isReplicaValid())
            return;
        //可以用信号连接replica的槽，或者invoke
        QMetaObject::invokeMethod(replica.data(), "setData", Qt::DirectConnection,
                                  Q_ARG(QString, "dynamic call setData:"+ui->editSend->text()));

    });
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::initConnection()
{
    //replica初始化完成后再进行信号槽绑定
    //因为元信息是动态获取的，所以只能使用宏的形式来连接信号槽
    //connect(replica.data(), SIGNAL(serverSignal(const QString &)), this, SLOT(clientSlot(const QString &)));
    //connect(this, SIGNAL(clientSignal(const QString &)), replica.data(), SLOT(serverSlot(const QString &)));
    connect(replica.data(), SIGNAL(dataChanged(const QString &)), this, SLOT(onDataChange(const QString &)));
}

void ClientWindow::onDataChange(const QString &str)
{
    ui->editRecv->append(str);
}

