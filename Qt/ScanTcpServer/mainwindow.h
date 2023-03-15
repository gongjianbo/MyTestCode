#pragma once
#include <QMainWindow>
#include "ScanTcpTask.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void cancel();
    void appendAddress(const QString &ip, const QString &hostName);

private:
    Ui::MainWindow *ui;

    ScanTcpTask *currentTask{nullptr};
};

