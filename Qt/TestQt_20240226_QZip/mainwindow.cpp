#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <functional>
#include "ZipTool.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnZipFile, &QPushButton::clicked, this, &MainWindow::zipFile);
    connect(ui->btnZipDir, &QPushButton::clicked, this, &MainWindow::zipDir);
    connect(ui->btnUnzip, &QPushButton::clicked, this, &MainWindow::unzip);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::zipFile()
{
    QString select_path = QFileDialog::getOpenFileName(this);
    if (select_path.isEmpty())
        return;
    QZipWriter zip(QApplication::applicationDirPath() + "/test.zip");
    QFile f(select_path);
    if (f.open(QIODevice::ReadOnly)) {
        QFileInfo info(select_path);
        zip.addFile(info.fileName(), f.readAll());
        f.close();
    }
    zip.close();
}

void MainWindow::zipDir()
{
    QString select_path = QFileDialog::getExistingDirectory(this);
    if (select_path.isEmpty())
        return;
    QZipWriter zip(QApplication::applicationDirPath() + "/test.zip");
    // 递归目录
    std::function<void(const QString &, const QString &)> find =
        [&](const QString &path, const QString &folder)
    {
        QDir path_dir(path);
        path_dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        path_dir.setSorting(QDir::DirsLast);
        auto &&info_list = path_dir.entryInfoList();
        for (int i = 0; i < info_list.size(); i++)
        {
            auto &&info = info_list.at(i);
            if (info.isDir()) {
                // qDebug()<<"dir"<<info.fileName()<<info.filePath();
                const QString sub_dir = folder + info.fileName() + "/";
                zip.addDirectory(sub_dir);
                find(info.filePath(), sub_dir);
            } else {
                // qDebug()<<"file"<<info.fileName()<<info.filePath();
                QFile f(info.filePath());
                if (f.open(QIODevice::ReadOnly)) {
                    zip.addFile(folder + info.fileName(), f.readAll());
                    f.close();
                }
            }
        }
    };
    find(select_path, "");

    zip.close();
}

void MainWindow::unzip()
{
    QString unzip_path = QApplication::applicationDirPath() + "/unzip";
    QZipReader zip(QApplication::applicationDirPath() + "/test.zip");
    QDir dir(unzip_path);
    if (!dir.exists()) {
        dir.mkpath(unzip_path);
    }
    zip.extractAll(unzip_path);
}
