#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("MainWindow ratio:%1").arg(QString::number(devicePixelRatioF(), 'f', 2)));
    connect(ui->loadImage, &QAction::triggered, this, &MainWindow::doLoadImage);

    ui->view1->setImage(QImage(":/flower.jpg"));
    ui->view2->setImage(QImage(":/flower.jpg"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doLoadImage()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (path.isEmpty())
        return;
    ui->view1->setImage(QImage(path));
    ui->view2->setImage(QImage(path));
}
