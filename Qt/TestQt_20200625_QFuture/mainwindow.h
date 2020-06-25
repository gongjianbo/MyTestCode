#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//QFuture类表示异步计算的结果
#include <QFuture>
//QFuture本身不带信号槽，可使用QFutureWatcher进行监控
#include <QFutureWatcher>
//QFutureInterface没有提供文档，出现于Qt源码
//QFuture与QFutureInterface的关系类似std::future与std::promise
//QFutureInterface可用于生成QFuture
//在Qt类实现中，QFuture持有一个QFutureInterface成员
#include <QFutureInterface>

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
    //QFuture本身不带信号槽，可使用QFutureWatcher进行监控
    QFutureWatcher<bool> myWatcher;
};
#endif // MAINWINDOW_H
