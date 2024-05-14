#pragma once
#include <QMainWindow>
#include "MyNode.h"

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

    void login();
    void logout();
    void sendRequest();
    void selectImage();

private:
    Ui::ClientWindow *ui;
    MyNode node;
};
