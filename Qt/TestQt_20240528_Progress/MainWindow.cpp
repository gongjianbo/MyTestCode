#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtConcurrentRun>
#include <QScopeGuard>
#include <QThread>
#include <QTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnStop->setEnabled(false);
    ui->progressBar->setRange(0, 100);
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::start);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::stop);
    // 同步进度值
    connect(&progress, &ProgressSync::valueChanged, [this](qreal value){
        ui->progressBar->setValue(value * 100);
    });
    // 同步状态
    connect(&progress, &ProgressSync::statusChanged, [this](int status){
        if (status != ProgressSync::Running) {
            ui->btnStart->setEnabled(true);
            ui->btnStop->setEnabled(false);
        } else {
            ui->btnStart->setEnabled(false);
            ui->btnStop->setEnabled(true);
        }
        switch (status) {
        case ProgressSync::Running:
            qDebug()<<"Running";
            break;
        case ProgressSync::Cancel:
            qDebug()<<"Cancel";
            break;
        case ProgressSync::Finish:
            qDebug()<<"Finish";
            break;
        case ProgressSync::Error:
            qDebug()<<"Error";
            break;
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{
    // start 初始化状态，并将 emitter 传入任务线程
    auto &&emitter = progress.start();
    QtConcurrent::run([this, emitter]{
        qDebug()<<"线程启动，任务开始";
        auto &&cleanup = qScopeGuard([emitter]{
            // 结束时调用 end 同步状态
            emitter->end();
        });
        Q_UNUSED(cleanup)
        const int count = 100;
        static bool err = true;
        err = !err;
        // 根据 running 状态判断是否取消
        for (int i = 0; i < count && emitter->getRunning(); i++)
        {
            QThread::msleep(50);
            // 调用 update 更新进度
            emitter->update((i + 1) / (double)count);
            // 模拟遇到错误，调用 error 更新状态
            if (err && i == 92) {
                emitter->error();
                break;
            }
        }
    });
}

void MainWindow::stop()
{
    progress.cancel();
}
