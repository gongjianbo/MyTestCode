#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ProcessManager.h"
#include <QCoreApplication>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 选择路径
    ui->editPath->setText(qApp->applicationDirPath() + "/SubProcess.exe");
    connect(ui->btnPath, &QPushButton::clicked, this, [=] {
        const QString exe_path = QFileDialog::getOpenFileName(this);
        if (exe_path.isEmpty())
            return;
        ui->editPath->setText(exe_path);
    });

    // 启动
    connect(ui->btnRun, &QPushButton::clicked, this, [=] {
        const QString exe_path = ui->editPath->text();
        const QString exe_key = ui->editKey->text();
        if (exe_path.isEmpty() || exe_key.isEmpty())
            return;
        const QString exe_arg = ui->editArg->text();
        // 传key过去作为server名
        const QStringList arg_list = QStringList() << exe_key << exe_arg;
        const bool exe_visible = ui->boxVisible->isChecked();
        const bool exe_restart = ui->boxRestart->isChecked();
        ProcessManager::getInstance()->startProcess(exe_path, exe_key, arg_list, exe_visible, exe_restart);
    });

    // 进程key列表
    connect(ProcessManager::getInstance(), &ProcessManager::keysChanged, this, [this](const QStringList &keys) {
        ui->boxKey->clear();
        ui->boxKey->addItems(keys);
    });

    // 结束
    connect(ui->btnStop, &QPushButton::clicked, this, [=] {
        const QString exe_key = ui->boxKey->currentText();
        if (exe_key.isEmpty())
            return;
        ProcessManager::getInstance()->stopProcess(exe_key);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

