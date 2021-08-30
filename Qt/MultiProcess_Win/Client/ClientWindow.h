#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private:
    Ui::ClientWindow *ui;
};

