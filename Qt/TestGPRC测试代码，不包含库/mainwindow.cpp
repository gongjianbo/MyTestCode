#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <thread>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //启动server
    connect(ui->btnStart,&QPushButton::clicked,[this]{
        std::thread th([this]{
            TestServer service;
            grpc::EnableDefaultHealthCheckService(true);
            grpc::reflection::InitProtoReflectionServerBuilderPlugin();
            grpc::ServerBuilder builder;
            builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
            builder.RegisterService(&service);
            myserver=std::unique_ptr<grpc::Server>(builder.BuildAndStart());

            qDebug()<<"server start";
            myserver->Wait();
            qDebug()<<"server quit";
        });
        th.detach();
    });

    //结束server
    connect(ui->btnQuit,&QPushButton::clicked,[this]{
        if(myserver){
            myserver->Shutdown();
        }
    });

    //调用接口A
    connect(ui->btnCallA,&QPushButton::clicked,[this]{
        TestClient client;
        client.callA(count);
        count++;
    });

    //调用接口B
    connect(ui->btnCallB,&QPushButton::clicked,[this]{
        TestClient client;
        client.callB(count);
        count++;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

