#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Qt WebSokcet Demo: By 龚建波");
}

MainWindow::~MainWindow()
{
    delete ui;
}

