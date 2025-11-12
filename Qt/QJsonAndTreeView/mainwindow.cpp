#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    jsonModel = new JsonTreeModel(this);
    ui->treeView->setModel(jsonModel);

    initLoadDump();
    initEdit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initLoadDump()
{
    // 选择导入的文件路径
    connect(ui->btnLoadPath, &QPushButton::clicked, this, [this](){
        const QString jsonpath = QFileDialog::getOpenFileName(this, "File Path");
        if(jsonpath.isEmpty()) return;
        ui->editLoadPath->setText(jsonpath);
    });
    // 导入Json文件
    connect(ui->btnLoadJson, &QPushButton::clicked, this, [this](){
        const QString loadpath = ui->editLoadPath->text();
        if (loadpath.isEmpty()) return;
        // parseJson.loadJson(loadpath);
        jsonModel->loadJson(loadpath);
        ui->treeView->expandAll();
    });

    // 选择导出的文件路径
    connect(ui->btnDumpPath, &QPushButton::clicked, this, [this](){
        const QString jsonpath = QFileDialog::getSaveFileName(this, "Save Path");
        if (jsonpath.isEmpty()) return;
        ui->editDumpPath->setText(jsonpath);
    });
    // 导出Json文件
    connect(ui->btnDumpJson, &QPushButton::clicked, this, [this](){
        const QString dumppath = ui->editDumpPath->text();
        if (dumppath.isEmpty()) return;
        // parseJson.dumpJson(dumppath);
        jsonModel->dumpJson(dumppath);
    });
}

void MainWindow::initEdit()
{
    // 增删部分参照示例：editable tree
    //
    // 我的item默认为enum:none，但目前还没有delegate来自定义操作
    // 只在item的appendchilren里测试性的改成了enum:value

    // 添加节点
    connect(ui->btnInsert, &QPushButton::clicked, this, [this](){
        QModelIndex index = ui->treeView->selectionModel()->currentIndex();
        // top节点是json外层的[]或{}，不处理
        if (!index.isValid() || !index.parent().isValid())
            return;
        QAbstractItemModel *model = ui->treeView->model();
        // TODO 处理插入时的类型问题，array中插入不需要key
        if (!model->insertRow(index.row() + 1, index.parent()))
            return;
        updateIndex();
        // 修改insert的内容
        // for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        //     QModelIndex child = model->index(index.row() + 1, column, index.parent());
        //     model->setData(child, QVariant("[No data]"), Qt::EditRole);
        // }
    });
    // 添加子节点
    connect(ui->btnInsertChild, &QPushButton::clicked, this, [this](){
        QModelIndex index = ui->treeView->selectionModel()->currentIndex();
        // top节点是json外层的[]或{}，不处理
        if (!index.isValid())
            return;
        QAbstractItemModel *model = ui->treeView->model();
        if (model->columnCount(index) == 0) {
            if (!model->insertColumn(0, index))
                return;
        }
        // TODO 处理插入时的类型问题，array中插入不需要key
        if (!model->insertRow(0, index))
            return;
        // 如果不是对象或者数组，就转换成对象
        const auto item_type = model->data(index, JsonTreeItem::ItemTypeRole).toInt();
        if (item_type != JsonTreeItem::Object && item_type != JsonTreeItem::Array) {
            model->setData(index, QVariant(JsonTreeItem::Object), JsonTreeItem::ItemTypeRole);
        }
        // 修改insert的内容
        // for (int column = 0; column < model->columnCount(index); ++column) {
        //     QModelIndex child = model->index(0, column, index);
        //     model->setData(child, QVariant("[No data]"), Qt::EditRole);
        // }
        ui->treeView->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                        QItemSelectionModel::ClearAndSelect);
        updateIndex();
    });
    // 删除节点树
    connect(ui->btnRemove, &QPushButton::clicked, this, [this](){
        QModelIndex index = ui->treeView->selectionModel()->currentIndex();
        QAbstractItemModel *model = ui->treeView->model();
        if (model->removeRow(index.row(), index.parent()))
            updateIndex();
    });
}

void MainWindow::updateIndex()
{
    // 抄的示例
    // bool hasSelection = !ui->treeView->selectionModel()->selection().isEmpty();
    // removeRowAction->setEnabled(hasSelection);
    // removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = ui->treeView->selectionModel()->currentIndex().isValid();
    // insertRowAction->setEnabled(hasCurrent);
    // insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        ui->treeView->closePersistentEditor(ui->treeView->selectionModel()->currentIndex());

        int row = ui->treeView->selectionModel()->currentIndex().row();
        int column = ui->treeView->selectionModel()->currentIndex().column();
        if (ui->treeView->selectionModel()->currentIndex().parent().isValid())
            qDebug() << QString("Position: (%1,%2)").arg(row).arg(column);
        else
            qDebug() << QString("Position: (%1,%2) in top level").arg(row).arg(column);
    }
}
