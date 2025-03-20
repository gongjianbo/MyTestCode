#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    QString getIniDir();
    QString getIniPath(const QString &filename);
    QString myCodec(const QString &filepath);
    QString uchardetCodec(const QString &filepath);

private slots:
    // 自己判断编码
    void on_myTest_clicked();
    // QSettings ini读写
    void on_myRead_clicked();
    void on_myWrite_clicked();
    // uchardet检测编码
    void on_uchardetTest_clicked();
    // win32 ini读写
    void on_profileRead_clicked();
    void on_profileWrite_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
