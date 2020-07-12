#include "WebSocketServer.h"
#include "ui_WebSocketServer.h"

#include <QHostAddress>
#include <QDebug>

WebSocketServer::WebSocketServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebSocketServer)
{
    ui->setupUi(this);

    //构造：QWebSocketServer（const QString& serverName,QWebSocketServer::SslMode secureMode,QObject *parent=nullptr）
    //使用给定的serverName构造一个新的QWebSocketServer。
    //该服务器名称将在HTTP握手阶段被用来识别服务器。它可以为空，此时不会将服务器名称发送给客户端。
    //SslMode指示服务器是通过wss（SecureMode）还是ws（NonSecureMode）运行
    //QWebSocketServer::SecureMode服务器以安全模式运行（通过wss）
    //QWebSocketServer::NonSecureMode服务器以非安全模式运行（通过ws）
    server=new QWebSocketServer("Server",QWebSocketServer::NonSecureMode,this);

    //服务器监听
    ui->btnSend->setEnabled(false);
    connect(ui->btnListen,&QPushButton::clicked,[this](){
        if(ui->btnListen->text()!="Listen"){
            ui->btnSend->setEnabled(false);
            ui->btnListen->setText("Listen");
            clearClient();
            server->close();
        }else{
            QHostAddress address;
            if(ui->editAddress->text()=="Any"){
                address=QHostAddress::Any;
            }else{
                address=QHostAddress(ui->editAddress->text());
            }
            //判断是否连接上
            if(server->listen(address,ui->editPort->text().toUInt())){
                ui->btnSend->setEnabled(true);
                ui->btnListen->setText("Dislisten");
            }
        }
    });
    //新的连接进来
    connect(server,&QWebSocketServer::newConnection,this,&WebSocketServer::onNewConnection);
    //发送消息给客户端
    connect(ui->btnSend,&QPushButton::clicked,[this](){
        if(!ui->editSend->toPlainText().isEmpty())
            emit sendMessage(ui->editSend->toPlainText());
    });
}

WebSocketServer::~WebSocketServer()
{
    clearClient();
    server->close();
    delete ui;
}

void WebSocketServer::clearClient()
{
    for(int i=clientList.count()-1;i>=0;i--)
    {
        //qDebug()<<i;
        clientList.at(i)->disconnect();
        clientList.at(i)->close();
    }
    qDeleteAll(clientList);
    clientList.clear();
}

void WebSocketServer::onNewConnection()
{
    QWebSocket *socket=server->nextPendingConnection();
    if(!socket)
        return;
    clientList.push_back(socket);
    ui->editRecv->append(QString("[New Connect] Address:%1  Port:%2")
                         .arg(socket->peerAddress().toString())
                         .arg(socket->peerPort()));

    //收到消息
    connect(socket,&QWebSocket::textMessageReceived,[this](const QString &msg){
        ui->editRecv->append(msg);
    });
    //发送消息
    connect(this,&WebSocketServer::sendMessage,socket,&QWebSocket::sendTextMessage);
    //断开连接，释放
    connect(socket,&QWebSocket::disconnected,[this,socket](){
        clientList.removeAll(socket);
        socket->deleteLater();
    });
}
