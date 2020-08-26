#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

/**
 * @brief 展示自定义的QHeaderView，以支持取消排序状态
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initTable();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
