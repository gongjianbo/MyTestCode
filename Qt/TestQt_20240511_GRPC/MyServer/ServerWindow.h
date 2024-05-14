#pragma once
#include <QMainWindow>
#include "MyService.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

    void startServer();
    void stopServer();

private:
    Ui::ServerWindow *ui;
    MyService service;
    std::unique_ptr<grpc::Server> server;
};
