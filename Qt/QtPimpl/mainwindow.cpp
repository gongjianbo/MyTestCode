#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label->addText("MyLabel");

    connect(ui->btnPrev,&QPushButton::clicked,ui->label,&MyLabel::toPrev);
    connect(ui->btnNext,&QPushButton::clicked,ui->label,&MyLabel::toNext);
    connect(ui->btnCurrent,&QPushButton::clicked,[=]{
        ui->label->addText(ui->lineEdit->text());
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
