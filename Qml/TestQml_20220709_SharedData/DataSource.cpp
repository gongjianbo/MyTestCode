#include "DataSource.h"
#include <QDebug>

int DataInfo::counter = 0;

DataInfo::DataInfo(const QString &str, int val)
    : key(str), value(val)
{
    id = ++counter;
    qDebug()<<__FUNCTION__<<id;
}

DataInfo::~DataInfo()
{
    qDebug()<<__FUNCTION__<<id;
}

DataSource::DataSource(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QSharedPointer<DataInfo>>("QSharedPointer<DataInfo>");
}

DataSource::~DataSource()
{

}

DataSource *DataSource::getInstance()
{
    static DataSource instance;
    return &instance;
}

void DataSource::deleteItem(QSharedPointer<DataInfo> item)
{
    if(dataList.removeOne(item)){
        emit itemDeleteNotify(item);
    }else{
        qDebug()<<__FUNCTION__<<"error";
    }
}

void DataSource::appendItem(const QString &key, int value)
{
    auto new_item = QSharedPointer<DataInfo>(new DataInfo(key, value));
    appendItem(new_item);
}

void DataSource::appendItem(QSharedPointer<DataInfo> item)
{
    dataList.append(item);
    emit itemAppendNotify(item);
}

void DataSource::updateItem(QSharedPointer<DataInfo> item)
{
    //在外部已经修改了，这里只是通知相关view更新
    int index = dataList.indexOf(item);
    if(index >= 0){
        emit itemUpdateNotify(item);
    }else{
        qDebug()<<__FUNCTION__<<"error";
    }
}

QList<QSharedPointer<DataInfo>> DataSource::searchItemByValue(int min, int max)
{
    QList<QSharedPointer<DataInfo>> group;
    for(auto item : dataList)
    {
        if(item->value >= min && item->value <= max)
            group.append(item);
    }
    return group;
}
