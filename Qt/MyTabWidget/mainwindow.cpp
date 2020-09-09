#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MyTabPage.h"
#include <QTabBar>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //主页不关闭
    ui->tabWidget->setFixedIndex(0);
    //添加一个tab页
    connect(ui->btnAppend,&QPushButton::clicked,[=]{
        MyTabPage *page=new MyTabPage;
        ui->tabWidget->appendNormalPage(page);
    });
    //移除当前展示的tab页
    connect(ui->btnRemove,&QPushButton::clicked,ui->tabWidget,&MyTabWidget::removeCurrentPage);
    //清除所有tab页
    connect(ui->btnClear,&QPushButton::clicked,ui->tabWidget,&MyTabWidget::clearNormalPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

