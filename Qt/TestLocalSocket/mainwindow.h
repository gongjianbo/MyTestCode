#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VoicePrintClient.h"
#include "VoicePrintSource.h"
#include "LocalHost.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    VoicePrintClient *client{ nullptr };
    VoicePrintSource *source{ nullptr };
    LocalHost *host{ nullptr };
};
#endif // MAINWINDOW_H
