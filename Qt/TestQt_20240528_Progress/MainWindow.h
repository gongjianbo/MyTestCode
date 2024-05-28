#pragma once
#include <QMainWindow>
#include "ProgressSync.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void start();
    void stop();

private:
    Ui::MainWindow *ui;
    ProgressSync progress;
};
