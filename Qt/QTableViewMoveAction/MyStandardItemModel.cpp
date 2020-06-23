#include "MyStandardItemModel.h"

#include <QMimeData>
#include <QDebug>

MyStandardItemModel::MyStandardItemModel(QObject *parent)
    : QStandardItemModel(parent)
{

}

Qt::ItemFlags MyStandardItemModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return Qt::ItemIsDragEnabled  | Qt::ItemIsDropEnabled  | QStandardItemModel::flags(index);
    return QStandardItemModel::flags(index);
}

Qt::DropActions MyStandardItemModel::supportedDropActions() const
{
    return Qt::MoveAction | QStandardItemModel::supportedDropActions();
}

QMimeData *MyStandardItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *data=QStandardItemModel::mimeData(indexes);
    if(data){
        // parent mimeData中已判断indexes有效性，无效的会返回nullptr
        // 也可以把信息放到model的mutable成员中
        data->setData("row",QByteArray::number(indexes.at(0).row()));
        data->setData("col",QByteArray::number(indexes.at(0).column()));
    }
    return data;
}

bool MyStandardItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if(!data||action!=Qt::MoveAction)
        return false;

    //这里没有判断toint ok（数据转换有效性）
    const QModelIndex old_index=index(data->data("row").toInt(),
                                      data->data("col").toInt());
    const QModelIndex current_index=parent;
    //可以先对index有效性进行判断，无效返回false，此处略过
    QStandardItem *old_item=takeItem(old_index.row(),old_index.column());
    QStandardItem *current_item=takeItem(current_index.row(),current_index.column());
    //交换两个item
    setItem(old_index.row(),old_index.column(),current_item);
    setItem(current_index.row(),current_index.column(),old_item);
    return true;
}
