#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "MyWorker.h"
#include "MyEvent.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool event(QEvent *event) override;

signals:
    void signalDoing(const QString &str);

private:
    Ui::MainWindow *ui;

    QThread *thread;
    MyWorker *worker;
};
#endif // MAINWINDOW_H
