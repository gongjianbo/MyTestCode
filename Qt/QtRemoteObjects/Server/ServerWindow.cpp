#include "ServerWindow.h"
#include "ui_ServerWindow.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    //成员属性如果为空，server启动和client释放都会有警告
    //server警告：qt.remoteobjects: QRemoteObjectSourceBase: Cannot replicate a NULL object "subObject"
    //client警告：QCoreApplication::postEvent: Unexpected null receiver
    source.setSubObject(new MyObjectSimpleSource(this));
    connect(&source,&MySource::dataChanged,[this](const QString &data){
        ui->editRecv->append(source.getData());
    });

    //连接服务器
    connect(ui->btnOpen,&QPushButton::clicked,[this]{
        if(host)
            return;
        host = new QRemoteObjectHost(this);
        //host->setHostUrl(QUrl("tcp://127.0.0.1:12315"));
        host->setHostUrl(QUrl("local:qro_test"));
        host->enableRemoting(&source);
    });
    //断开连接
    connect(ui->btnClose,&QPushButton::clicked,[this]{
        if(!host)
            return;
        host->disableRemoting(&source);
        host->deleteLater();
        host = nullptr;
    });

    //发送
    connect(ui->btnSend,&QPushButton::clicked,[this]{
        source.setData("server setData:"+ui->editSend->text());
    });
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

