#include "SubWindow.h"
#include "ui_SubWindow.h"

SubWindow::SubWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SubWindow)
{
    ui->setupUi(this);
}

SubWindow::~SubWindow()
{
    delete ui;
}

