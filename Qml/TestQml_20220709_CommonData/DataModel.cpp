#include "DataModel.h"
#include <QDebug>

DataModel::DataModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto source = DataSource::getInstance();
    connect(source, &DataSource::itemAppendNotify,
            this, [this](QSharedPointer<DataInfo> item){
        //符合条件，插入
        if(item->value >= searchMin && item->value <= searchMax){
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            dataList.append(item);
            endInsertRows();
        }
    });
    connect(source, &DataSource::itemDeleteNotify,
            this, [this](QSharedPointer<DataInfo> item){
        //存在则移除
        int row = dataList.indexOf(item);
        if(row >= 0){
            beginRemoveRows(QModelIndex(), row, row);
            dataList.removeAt(row);
            endRemoveRows();
        }
    });
    connect(source, &DataSource::itemUpdateNotify,
            this, [this](QSharedPointer<DataInfo> item){
        int row = dataList.indexOf(item);
        if(row >= 0){
            //已存在，判断是否需要移除
            if(item->value >= searchMin && item->value <= searchMax){
                emit dataChanged(index(row, 0), index(row, 0));
            }else{
                beginRemoveRows(QModelIndex(), row, row);
                dataList.removeAt(row);
                endRemoveRows();
            }
        }else if(item->value >= searchMin && item->value <= searchMax){
            //不存在，判断是否需要重新查询
            searchItemByValue(searchMin, searchMax);
        }
    });
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return dataList.size();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    const auto &item = dataList.at(index.row());
    switch (role) {
    case KeyRole: return item->key;
    case ValueRole: return item->value;
    }
    return QVariant();
}

QHash<int, QByteArray> DataModel::roleNames() const
{
    return QHash<int, QByteArray>{
        { KeyRole, "key" }
        ,{ ValueRole, "value" }
    };
}

void DataModel::appendItem(const QString &key, int value)
{
    DataSource::getInstance()->appendItem(key, value);
}

void DataModel::deleteItemByRow(int row)
{
    if(row < 0 || row >= dataList.size())
        return;
    DataSource::getInstance()->deleteItem(dataList.at(row));
}

void DataModel::updateItemByRow(int row, const QString &key, int value)
{
    if(row < 0 || row >= dataList.size())
        return;
    auto item = dataList.at(row);
    item->key = key;
    item->value = value;
    DataSource::getInstance()->updateItem(item);
}

void DataModel::searchItemByValue(int min, int max)
{
    searchMin = min;
    searchMax = max;
    auto item_list = DataSource::getInstance()->searchItemByValue(min, max);
    beginResetModel();
    dataList = item_list;
    endResetModel();
}
