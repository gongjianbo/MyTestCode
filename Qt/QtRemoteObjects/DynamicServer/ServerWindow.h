#pragma once
#include <QMainWindow>
#include <QRemoteObjectHost>
#include "MySource.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE


class ServerWindow : public QMainWindow
{
    Q_OBJECT
public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

private:
    Ui::ServerWindow *ui;

    QRemoteObjectHost host;
    MySource source;
};
