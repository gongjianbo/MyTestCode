#include "AbnormalSignalModel.h"

#include <QDebug>

AbnormalSignalModel::AbnormalSignalModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    RowArray row={}; //默认初始化
    //for(auto item:row){qDebug()<<item;}
    for(int i=0;i<rows;i++){
        modelItems.append(row);
    }
}

int AbnormalSignalModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return modelItems.count();
}

int AbnormalSignalModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return cols;
}

QVariant AbnormalSignalModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch (role) {
    case ItemRole:
        return QVariant(modelItems.at(index.row()-1).at(index.column()-1).abnormal);
        break;
    default:
        break;
    }

    return QVariant();
}

