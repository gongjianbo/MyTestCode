#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <atomic>
#include <mutex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//测试qt中sqlite多线程写入操作
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createTable();
    void clearTable();
    void searchCount();
    void writeTest(int index,int loop,bool lock);

private:
    Ui::MainWindow *ui;

    std::mutex mtx;
    std::atomic_bool stopFlag=true;
};
#endif // MAINWINDOW_H
