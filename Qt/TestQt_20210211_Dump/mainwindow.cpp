#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton,&QPushButton::clicked,
            this,[]{
        int i=0;
        int j=100/i;
        j;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

