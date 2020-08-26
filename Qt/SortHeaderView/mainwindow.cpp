#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SortHeaderView.h"
#include <QRandomGenerator>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTable()
{
    //设置自定义的表头
    SortHeaderView *header=new SortHeaderView(Qt::Horizontal,ui->tableView);
    ui->tableView->setHorizontalHeader(header);
    //可以交换列
    header->setSectionsMovable(true);

    //单元格默认宽高
    ui->tableView->verticalHeader()->setDefaultSectionSize(24);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(50);

    //数据model
    QStandardItemModel* model = new QStandardItemModel(this);
    const int col_max=10;
    const int row_max=20;
    //设置列表头
    model->setColumnCount(col_max);
    for(int i=0;i<col_max;i++)
        model->setHeaderData(i,Qt::Horizontal, QString("Col %1").arg(i));
    //设置行表头
    model->setRowCount(row_max);
    for(int i=0;i<row_max;i++)
        model->setHeaderData(i,Qt::Vertical, QString("Row %1").arg(i));

    //使用随机数填充
    QRandomGenerator *rand_gen=QRandomGenerator::global();
    int value=0;
    for(int i=0;i<row_max;i++){
        for(int j=0;j<col_max;j++){
            //第一列用行数，其他的用随机数
            value=(j==0)?(i): (rand_gen->bounded(0,100));
            QStandardItem *new_item=new QStandardItem(QString::number(value));
            //用文本排序的话，默认是字典顺序
            new_item->setData(value,Qt::InitialSortOrderRole);
            //文本居中
            new_item->setData(Qt::AlignCenter,Qt::TextAlignmentRole);
            model->setItem(i, j, new_item);
        }
    }

    //view会根据model提供的数据来渲染
    //ui->tableView->setModel(model);
    //屏蔽编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置排序
    //给普通的 ItemModel 加一层 SortProxy
    QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel(this);
    //默认排序role应该是dispalyrole
    proxy_model->setSortRole(Qt::InitialSortOrderRole);
    proxy_model->setSourceModel(model);
    ui->tableView->setModel(proxy_model);
    //允许点击排序
    ui->tableView->setSortingEnabled(true);
    //按第0列升序
   // ui->tableView->sortByColumn(0,Qt::AscendingOrder);

    //排序自定义信号
    //如果是自定义view，这段加到view里去
    connect(header, &SortHeaderView::sortOrderChanged,
            this, [this,header,proxy_model](int logicIndex,SortHeaderView::SortOrder order){
        QTableView *view=ui->tableView;
        //无效index或NoOrder就设置为默认未排序状态
        if (logicIndex < 0 || order == SortHeaderView::NoOrder){
            //去掉排序三角样式
            header->setSortIndicator(-1, Qt::DescendingOrder);
            //QSortFilterProxyModel can be sorted by column -1,
            //in which case it returns to the sort order of the underlying source model.
            //-1则还原model默认顺序
            proxy_model->sort(-1, Qt::DescendingOrder);
        }
        else{
            switch (order) {
            case SortHeaderView::DescOrder:
                view->sortByColumn(logicIndex, Qt::DescendingOrder);
                break;
            case SortHeaderView::AscOrder:
                view->sortByColumn(logicIndex, Qt::AscendingOrder);
                break;
            }
        }
    });
}


