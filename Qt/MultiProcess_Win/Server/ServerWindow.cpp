#include "ServerWindow.h"
#include "ui_ServerWindow.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

