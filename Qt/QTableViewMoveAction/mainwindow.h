#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initMyTableModel();
    void initMyStandardItemModel();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
