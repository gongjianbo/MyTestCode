#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtAV>
#include <QtAVWidgets>
#include <QtAV/AVTranscoder.h>

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
    QtAV::AVTranscoder *theAvt;
    //QtAV::VideoPreviewWidget
};

#endif // MAINWINDOW_H
