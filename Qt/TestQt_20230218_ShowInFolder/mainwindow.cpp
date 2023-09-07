#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDesktopServices>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if defined(Q_OS_WIN)
    connect(ui->btnCom, &QPushButton::clicked, [=]{
        byCom(QApplication::applicationDirPath() + "/文件夹/文件名A.txt");
    });
#endif
    // 如果用 explorer 打开，路径带特殊符号或中文，第一次打开可能无法选中文件
    connect(ui->btnExplorer, &QPushButton::clicked, [=]{
        byExplorer(QApplication::applicationDirPath() + "/文件夹/文件名B.txt");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

#if defined(Q_OS_WIN)
#include <Windows.h>
#include <objbase.h>
#pragma comment(lib, "Ole32.lib")
#include <ShlObj_core.h>
#pragma comment(lib, "Shell32.lib")
void MainWindow::byCom(const QString &filepath)
{
    ::CoInitialize(nullptr);
    QString path = QDir::toNativeSeparators(filepath);
    std::wstring str = path.toStdWString();
    PIDLIST_ABSOLUTE pidl = ::ILCreateFromPathW((PCWSTR)str.c_str());
    if(pidl){
        ::SHOpenFolderAndSelectItems(pidl,0,0,0);
        ::ILFree(pidl);
    }
    ::CoUninitialize();
}
#endif

// https://github.com/wireshark/wireshark
void MainWindow::byExplorer(const QString &filepath)
{
    bool success = false;

// https://stackoverflow.com/questions/3490336/how-to-reveal-in-finder-or-show-in-explorer-with-qt

#if defined(Q_OS_WIN)
    QString command = "explorer.exe";
    QStringList arguments;
    QString path = QDir::toNativeSeparators(filepath);
    arguments << "/select," << path + "";
    success = QProcess::startDetached(command, arguments);
#elif defined(Q_OS_MAC)
    QStringList script_args;
    QString escaped_path = filepath;

    escaped_path.replace('"', "\\\"");
    script_args << "-e"
                << QString("tell application \"Finder\" to reveal POSIX file \"%1\"")
                       .arg(escaped_path);
    if (QProcess::execute("/usr/bin/osascript", script_args) == 0) {
        success = true;
        script_args.clear();
        script_args << "-e"
                    << "tell application \"Finder\" to activate";
        QProcess::execute("/usr/bin/osascript", script_args);
    }
#else
    // Is there a way to highlight the file using xdg-open?
#endif
    if (!success) {
        QFileInfo file_info(filepath);
        // 参数是文件夹会在资源管理器打开，如果参数是文件会用默认打开方式打开
        QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.dir().absolutePath()));
    }
}

