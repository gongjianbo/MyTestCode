#include "ServerWindow.h"
#include "ui_ServerWindow.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);
    ui->btnClose->setEnabled(false);

    connect(&source,&MySource::dataChanged,[this](const QString &data){
        //source.getData()
        ui->editRecv->append(data);
    });

    //连接服务器
    connect(ui->btnOpen,&QPushButton::clicked,[this]{
        //host = new QRemoteObjectHost(this);
        //host->setHostUrl(QUrl("tcp://127.0.0.1:12315"));
        host.setHostUrl(QUrl("local:qro_test"));
        //可以设置enableRemoting的name，和node.acquire的name配对
        host.enableRemoting(&source, "QRO");
        //更新按钮状态
        ui->btnOpen->setEnabled(false);
        ui->btnClose->setEnabled(true);
    });
    //断开连接
    connect(ui->btnClose,&QPushButton::clicked,[this]{
        host.disableRemoting(&source);
        //更新按钮状态
        ui->btnOpen->setEnabled(true);
        ui->btnClose->setEnabled(false);
    });

    //测试通知客户端
    connect(ui->btnNotify,&QPushButton::clicked,[this]{
        emit source.dataChanged("dynamic notify dataChanged:"+ui->editSend->text());
    });
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

