#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnScan, &QPushButton::clicked, [=](){
        cancel();
        ui->btnScan->setEnabled(false);
        ui->listWidget->clear();

        ScanTcpTask *task = new ScanTcpTask(12315, true, this);
        connect(task, &ScanTcpTask::finished, [task, this]()mutable{
            if (task == currentTask) {
                currentTask = nullptr;
            }
            task->deleteLater();
            task = nullptr;
        });
        connect(task, &ScanTcpTask::foundAddress, this, &MainWindow::appendAddress);
        connect(task, &ScanTcpTask::scanFinished, this, [this](){ ui->btnScan->setEnabled(true); });
        task->start();
        currentTask = task;
    });
    connect(ui->btnCancel, &QPushButton::clicked, this, &MainWindow::cancel);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (currentTask) {
        currentTask->disconnect(this);
        currentTask->quit();
        currentTask->wait();
    }
    currentTask = nullptr;
}

void MainWindow::cancel()
{
    if (currentTask) {
        currentTask->disconnect(this);
        currentTask->quit();
        // 考虑到多次执行，这里不阻塞
        //currentTask->wait();
    }
    currentTask = nullptr;
    ui->btnScan->setEnabled(true);
}

void MainWindow::appendAddress(const QString &ip, const QString &hostName)
{
    ui->listWidget->addItem(QString("IP: %1  HostName: %2").arg(ip).arg(hostName));
}

