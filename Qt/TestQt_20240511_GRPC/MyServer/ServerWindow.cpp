#include "ServerWindow.h"
#include "ui_ServerWindow.h"
#include <QDebug>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    connect(&service, &MyService::updateLog, ui->editLog, &QTextEdit::append);
    connect(ui->btnStart, &QPushButton::clicked, this, &ServerWindow::startServer);
    connect(ui->btnStop, &QPushButton::clicked, this, &ServerWindow::stopServer);
}

ServerWindow::~ServerWindow()
{
    delete ui;
    stopServer();
}

void ServerWindow::startServer()
{
    stopServer();
    QString host = ui->editHost->text();
    std::thread th([=]{
        service.initService();
        grpc::EnableDefaultHealthCheckService(true);
        grpc::ServerBuilder builder;
        builder.AddListeningPort(host.toStdString(), grpc::InsecureServerCredentials());
        // 消息默认限制 4M
        const int max_size = 1024 * 1024 * 1024;
        builder.SetMaxReceiveMessageSize(max_size);
        builder.SetMaxSendMessageSize(max_size);
        // builder.SetSyncServerOption(grpc::ServerBuilder::MIN_POLLERS, 2);
        builder.RegisterService(&service);
        server = std::unique_ptr<grpc::Server>(builder.BuildAndStart());

        qDebug()<<"server start";
        server->Wait();
        qDebug()<<"server quit";
    });
    th.detach();
}

void ServerWindow::stopServer()
{
    if (!server)
        return;
    server->Shutdown();
    server.reset();
}
