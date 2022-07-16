#include "MyTreeModel.h"

MyTreeModel::MyTreeModel(QObject *parent)
    : QAbstractItemModel(parent), rootItem(new MyTreeItem)
{
}

QModelIndex MyTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();
    MyTreeItem *parentItem = getItem(parent);
    auto childPtr = parentItem->subItems.at(row);
    if (childPtr){
        return createIndex(row, column, childPtr.get());
    }else{
        return QModelIndex();
    }
}

QModelIndex MyTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid()){
        return QModelIndex();
    }
    MyTreeItem *childItem = getItem(index);
    auto parentPtr = childItem->parentItem;
    if(!parentPtr || parentPtr == rootItem){
        return QModelIndex();
    }
    return createIndex(parentPtr.get()->row, 0, parentPtr.get());
}

int MyTreeModel::rowCount(const QModelIndex &parent) const
{
    MyTreeItem *parentItem = getItem(parent);
    return parentItem->subItems.size();
}

int MyTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant MyTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    MyTreeItem *item = getItem(index);
    if(role == Qt::DisplayRole){
        //TreeView继承自TableView，所以可以通过不同的column来取单元格数据
        role = Qt::UserRole + index.column();
    }
    switch (role) {
    case NameRole: return item->name;
    case DetailRole: return item->detail;
    default: break;
    }
    return QVariant();
}

QHash<int, QByteArray> MyTreeModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names.insert(QHash<int, QByteArray>{
                     {NameRole, "name"}
                     ,{DetailRole, "detail"}
                 });
    return names;
}

void MyTreeModel::resetItems()
{
    beginResetModel();
    for(int i=0; i<10; i++)
    {
        QSharedPointer<MyTreeItem> lv1{new MyTreeItem};
        lv1->parentItem = rootItem;
        rootItem->subItems.append(lv1);
        lv1->row = i;
        lv1->name = QString("lv1-%1").arg(i);
        lv1->detail = QString("detail-%1").arg(i);
        for(int j=0; j<10; j++)
        {
            QSharedPointer<MyTreeItem> lv2{new MyTreeItem};
            lv2->parentItem = lv1;
            lv1->subItems.append(lv2);
            lv2->row = j;
            lv2->name = QString("lv2-%1-%2").arg(i).arg(j);
            lv2->detail = QString("detail-%1-%2").arg(i).arg(j);
        }
    }
    endResetModel();
    qDebug()<<__FUNCTION__<<rowCount()<<columnCount();
}

MyTreeItem *MyTreeModel::getItem(const QModelIndex &idx) const
{
    if(idx.isValid()){
        MyTreeItem *item = static_cast<MyTreeItem*>(idx.internalPointer());
        if(item){
            return item;
        }
    }
    return rootItem.get();
}

