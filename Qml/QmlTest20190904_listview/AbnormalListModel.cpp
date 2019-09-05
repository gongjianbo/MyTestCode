#include "AbnormalListModel.h"

#include <QDebug>

AbnormalListModel::AbnormalListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    RowData row={};
    QTime time=QTime(QTime::currentTime().hour(),0); //当前时间

    for(int i=0;i<colLimit;i++){
        row[i]=i%3-1;
    }
    //for(auto item:row){qDebug()<<item;}
    TableData table1;
    table1.freq=10;
    for(int i=0;i<rowLimit;i++){
        table1.tableRows.append(row);
        table1.tableHour.append(time.addSecs(i*60*60)); //+i
    }

    TableData table2;
    table2.freq=30;
    for(int i=0;i<20;i++){
        table2.tableRows.append(row);
        table2.tableHour.append(time.addSecs(i*60*60)); //+i
    }

    modelData.append(table1);
    modelData.append(table2);
}

int AbnormalListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    //有parent的返回0，这样他就不会成为tree model
    //if (parent.isValid())return 0;
    Q_UNUSED(parent)
    if(indexValid(currentIndex,0,0))
        return modelData.at(currentIndex).tableRows.count();
    return 0;
}

int AbnormalListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return colLimit;
}

QVariant AbnormalListModel::data(const QModelIndex &index, int role) const
{
    //qDebug()<<index<<role;
    if (!index.isValid())
        return QVariant();

    const int row=index.row();

    switch (role) {
    case HourRole:
        return modelData.at(currentIndex).tableHour.at(row).toString("hh:mm");
        break;
    default:
        break;
    }

    return QVariant();
}

int AbnormalListModel::getCurrentIndex() const
{
    return currentIndex;
}

void AbnormalListModel::setCurrentIndex(int index)
{
    beginResetModel();
    currentIndex=index;
    endResetModel();
    emit currentIndexChanged();
}

int AbnormalListModel::abnormal(int row, int col) const
{
    if(indexValid(currentIndex,row,col)){
        return (modelData.at(currentIndex).tableRows.at(row).at(col));
    }
    return false;
}

QStringList AbnormalListModel::groupList() const
{
    QStringList group;
    for(auto &item:modelData){
        group.append(QString::number(item.freq,'f',3));
    }
    return group;
}

bool AbnormalListModel::indexValid(int index,int row, int col) const
{
    return (index>=0&&index<modelData.count()&&
            row>=0&&row<modelData.at(index).tableRows.count()&&
            col>=0&&col<colLimit);
}

