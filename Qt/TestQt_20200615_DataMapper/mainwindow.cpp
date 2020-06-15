#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MyMapper.h"
#include <QDataWidgetMapper>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initQtMapper();
    initMyMapper();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initQtMapper()
{
    //QDataWidgetMapper提供数据model与widget属性之间的映射
    //示例搜mapper有简单的例子
    QDataWidgetMapper *mapper=new QDataWidgetMapper(this);
    //可以属性改变时更新到model，或者手动submit
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    //mapper是靠属性来获取和设置值的
    //combobox没有items属性，就展示切换index吧
    //也可以派生给combox加个items属性
    QStringList items{"A","B","C"};
    ui->comboBox->addItems(items);

    //QDataWidgetMapper接受QAbstractItemModel作为model
    //这里我使用派生类QStandardItemModel
    QStandardItemModel *model=new QStandardItemModel;
    for(int row=0;row<3;row++)
    {
        QStandardItem *item1=new QStandardItem;
        item1->setData(items.at(row),Qt::DisplayRole);
        model->setItem(row,0,item1);
        QStandardItem *item2=new QStandardItem;
        item2->setData("Test"+QString::number(row),Qt::DisplayRole);
        model->setItem(row,1,item2);
        QStandardItem *item3=new QStandardItem;
        item3->setData(10+row,Qt::DisplayRole);
        model->setItem(row,2,item3);
    }
    mapper->setModel(model);

    //关联widget
    //可以指定绑定的属性
    mapper->addMapping(ui->comboBox,0,"currentText");
    mapper->addMapping(ui->lineEdit,1);
    mapper->addMapping(ui->spinBox,2);
    mapper->toFirst();

    //保存修改
    connect(ui->btnSave,&QPushButton::clicked,mapper,&QDataWidgetMapper::submit);
    //上一section
    connect(ui->btnPrev,&QPushButton::clicked,mapper,&QDataWidgetMapper::toPrevious);
    //下一section
    connect(ui->btnNext,&QPushButton::clicked,mapper,&QDataWidgetMapper::toNext);
    //打印Model
    connect(ui->btnModel,&QPushButton::clicked,this,[this,model]{
        for(int row=0;row<model->rowCount();row++)
        {
            qDebug()<<model->data(model->index(row,0),Qt::DisplayRole)
                   <<model->data(model->index(row,1),Qt::DisplayRole)
                  <<model->data(model->index(row,2),Qt::DisplayRole);
        }
    });
}

void MainWindow::initMyMapper()
{
    MyMapper *mapper=new MyMapper(this);

    //mapper是靠属性来获取和设置值的
    //combobox没有items属性，就展示切换index吧
    //也可以派生给combox加个items属性
    QStringList items{"A2","B2","C2"};
    ui->comboBox->addItems(items);

    //model
    QStandardItemModel *model=new QStandardItemModel;
    for(int row=0;row<3;row++)
    {
        QStandardItem *item1=new QStandardItem;
        item1->setData(items.at(row),Qt::DisplayRole);
        model->setItem(row,0,item1);
        QStandardItem *item2=new QStandardItem;
        item2->setData("Test"+QString::number(row),Qt::DisplayRole);
        model->setItem(row,1,item2);
        QStandardItem *item3=new QStandardItem;
        item3->setData(10+row,Qt::DisplayRole);
        model->setItem(row,2,item3);
    }
    mapper->setModel(model);

    //关联widget
    //可以指定绑定的属性
    mapper->addMapping(ui->comboBox,0,"currentText");
    mapper->addMapping(ui->lineEdit,1,"text");
    mapper->addMapping(ui->spinBox,2,"value");
    mapper->toFirst();

    //保存修改
    connect(ui->btnSave,&QPushButton::clicked,mapper,&MyMapper::submit);
    //上一section
    connect(ui->btnPrev,&QPushButton::clicked,mapper,&MyMapper::toPrev);
    //下一section
    connect(ui->btnNext,&QPushButton::clicked,mapper,&MyMapper::toNext);
    //打印Model
    connect(ui->btnModel,&QPushButton::clicked,this,[this,model]{
        for(int row=0;row<model->rowCount();row++)
        {
            qDebug()<<model->data(model->index(row,0),Qt::DisplayRole)
                   <<model->data(model->index(row,1),Qt::DisplayRole)
                  <<model->data(model->index(row,2),Qt::DisplayRole);
        }
    });
}

