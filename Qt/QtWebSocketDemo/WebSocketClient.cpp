#include "WebSocketClient.h"
#include "ui_WebSocketClient.h"

#include <QHostAddress>
#include <QDebug>

WebSocketClient::WebSocketClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebSocketClient)
{
    ui->setupUi(this);

    //构造：QWebSocket(const QString &origin = QString(), QWebSocketProtocol::Version version = QWebSocketProtocol::VersionLatest, QObject *parent = nullptr)
    //使用给定的源，要使用的协议版本和parent创建一个新的QWebSocket 。
    client=new QWebSocket;
    client->setParent(this);

    //连接服务端
    connect(ui->btnOpen,&QPushButton::clicked,[this](){
        if(ui->btnOpen->text()!="Open"){
            client->close();
        }else{
            client->open(QUrl(ui->editUrl->text()));
        }
    });
    //连接结果
    ui->btnSend->setEnabled(false);
    connect(client,&QWebSocket::connected,[this](){
        ui->btnSend->setEnabled(true);
        ui->btnOpen->setText("Close");
        ui->footLabel->setText(QString("Address:%1  Port:%2")
                               .arg(client->localAddress().toString())
                               .arg(client->localPort()));
        qDebug()<<"connected";
    });
    connect(client,&QWebSocket::disconnected,[this](){
        ui->btnSend->setEnabled(false);
        ui->btnOpen->setText("Open");
        qDebug()<<"disconnected";
    });
    //发送数据
    connect(ui->btnSend,&QPushButton::clicked,[this](){
        if(!ui->editSend->toPlainText().isEmpty())
            client->sendTextMessage(ui->editSend->toPlainText());
    });
    //接收数据
    connect(client,&QWebSocket::textMessageReceived,[this](const QString &msg){
        ui->editRecv->append(msg);
    });
}

WebSocketClient::~WebSocketClient()
{
    //结束的时候没关会异常
    client->close();
    delete ui;
}
