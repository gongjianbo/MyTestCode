#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MyTableModel.h"
#include "MyStandardItemModel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMyTableModel();
    initMyStandardItemModel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMyTableModel()
{
    QTableView *view=ui->tableViewA;
    MyTableModel *model=new MyTableModel;
    view->setModel(model);

    view->setSelectionMode(QAbstractItemView::SingleSelection); //不是必要的
    //可以配合行/列选中，需要在Model中做相应处理
    //view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setDragEnabled(true);
    view->setDefaultDropAction(Qt::MoveAction); //不是必要的
    view->setDragDropMode(QAbstractItemView::InternalMove);
}

void MainWindow::initMyStandardItemModel()
{
    //QStandardItemModel类提供用于存储自定义数据的通用模型
    MyStandardItemModel* model = new MyStandardItemModel(this);

    const int row_count=10;
    const int col_count=6;
    //设置列表头
    model->setColumnCount(col_count);
    for(int col=0;col<col_count;col++){
        model->setHeaderData(col,Qt::Horizontal, QString::number(col));
    }

    //设置行表头
    model->setRowCount(row_count);
    for(int row=0;row<row_count;row++){
        model->setHeaderData(row,Qt::Vertical, QString::number(row));
    }
    //设置数据
    for(int row=0;row<row_count;row++)
    {
        for(int col=0;col<col_count;col++)
        {
            QStandardItem *new_item=new QStandardItem(
                        QString("%1 %2").arg(row).arg(col));
            model->setItem(row, col, new_item);
        }
    }


    QTableView *view=ui->tableViewB;
    view->setModel(model);

    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setDragEnabled(true);
    view->setDefaultDropAction(Qt::MoveAction);//不是必要的
    view->setDragDropMode(QAbstractItemView::InternalMove);
}
