#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //选择文件
    connect(ui->btnSelect, &QPushButton::clicked, [this]{
        const QString filepath = QFileDialog::getOpenFileName(this);
        if(filepath.isEmpty())
            return;
        ui->editFilepath->setText(filepath);
    });
    //上传
    connect(ui->btnUpload, &QPushButton::clicked, [this]{
        if(ui->editFilepath->text().isEmpty())
            return;
        const QString filepath = ui->editFilepath->text();
        const QString url = ui->editUrl->text()+"/upload";
        manager.upload(url,filepath);
    });
    //下载
    connect(ui->btnDownload, &QPushButton::clicked, [this]{
        if(ui->editFilepath->text().isEmpty())
            return;
        const QString filepath = ui->editFilepath->text();
        QFileInfo info(filepath);
        const QString url = ui->editUrl->text()+"/download";
        manager.download(url,"0",info.dir().absoluteFilePath("download_"+info.fileName()));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

