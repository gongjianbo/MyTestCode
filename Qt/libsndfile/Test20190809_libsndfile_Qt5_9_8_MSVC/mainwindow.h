#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBuffer>
#include <QFile>
#include <QEventLoop>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioOutput>

#include <sndfile.hh>

namespace Ui {
class MainWindow;
}

struct AudioPacket
{
    qint64 offset;
    QByteArray data;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void test(const QString &filepath);

private:
    Ui::MainWindow *ui;
    QEventLoop loop;
};

#endif // MAINWINDOW_H
