#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtAV>
#include <QtAVWidgets>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //QtAV::VideoOutput 会异常退出
    QtAV::VideoRenderer *theRender;
    QtAV::AVPlayer *thePlayer;
    //QtAV::VideoPreviewWidget
};

#endif // MAINWINDOW_H
