#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getIniPath(const QString &filename);

private slots:
    // QSettings 读写
    // QSettings有缓存，如果先用A编码读取，那再用B编码打开依然会用到之前的数据，文件修改后才会重新读取
    void on_qtWrite_clicked();
    void on_qtRead_clicked();
    // QSettings::registerFormat 自定义读写
    void on_customWrite_clicked();
    void on_customRead_clicked();
    // 自定义ini文件读写
    void on_easyWrite_clicked();
    void on_easyRead_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
