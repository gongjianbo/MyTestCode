#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //默认从右往左
    ui->rightToLeft->setDirection(ScrollLabel::RightToLeft);
    //设置为从左往右，并设置刷新间隔50ms
    ui->leftToRight->setDirection(ScrollLabel::LeftToRight);
    ui->leftToRight->setInterval(50);
}

MainWindow::~MainWindow()
{
    delete ui;
}

