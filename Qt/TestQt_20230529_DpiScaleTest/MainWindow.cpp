#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QPixmap>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_3->setPixmap(QPixmap(":/hehua.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
