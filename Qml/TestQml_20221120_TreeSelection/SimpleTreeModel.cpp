#include "SimpleTreeModel.h"
#include <QDebug>

SimpleTreeModel::SimpleTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    reset();
}

QVariant SimpleTreeModel::data(const QModelIndex &index, int role) const
{
    QStandardItem *item = itemFromIndex(index);
    if (!item)
        return QVariant();

    if (role == Qt::DisplayRole) {
        return item->data(role);
    }
    return QVariant();
}

void SimpleTreeModel::appendNode(const QModelIndex &index, const QString &text)
{
    QStandardItem *item = itemFromIndex(index);
    if (!item)
        return;
    QStandardItem *newitem = new QStandardItem(text);
    newitem->setData(text, Qt::DisplayRole);
    item->appendRow(newitem);
}

void SimpleTreeModel::removeNode(const QModelIndex &index)
{
    QStandardItem *item = itemFromIndex(index);
    if (!item)
        return;
    QStandardItem *parentitem = item->parent();
    if (parentitem) {
        parentitem->removeRow(item->row());
    } else {
        removeRow(item->row());
    }
}

void SimpleTreeModel::reset()
{
    beginResetModel();
    {
        const QSignalBlocker blocker(this); (void)blocker;
        for (int i = 0; i < 10; i++)
        {
            QStandardItem *top = new QStandardItem(QString("Top %1").arg(i));
            appendRow(top);
            for (int j = 0; j < 3; j++)
            {
                QStandardItem *sub = new QStandardItem(QString("Sub %1 %2").arg(i).arg(j));
                top->appendRow(sub);
                for (int k = 0; k < 3; k++)
                {
                    QStandardItem *inner = new QStandardItem(QString("In %1").arg(k));
                    sub->appendRow(inner);
                }
            }
        }
    }
    endResetModel();
}

void SimpleTreeModel::clear()
{
    beginResetModel();
    {
        const QSignalBlocker blocker(this); (void)blocker;
        if (rowCount() > 0)
            removeRows(0, rowCount());
    }
    endResetModel();
}
