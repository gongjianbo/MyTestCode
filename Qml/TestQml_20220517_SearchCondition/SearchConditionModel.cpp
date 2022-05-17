#include "SearchConditionModel.h"
#include <QDebug>

QDataStream& operator<<(QDataStream& out, const SearchCondition& item)
{
    out<<item.c1MatchAll<<item.c1Keywords<<item.c2MatchAll<<item.c2Keywords
      <<item.enableC2<<item.matchAll;
    return out;
}

QDataStream& operator>>(QDataStream& in, SearchCondition& item)
{
    in>>item.c1MatchAll>>item.c1Keywords>>item.c2MatchAll>>item.c2Keywords
            >>item.enableC2>>item.matchAll;
    return in;
}

SearchConditionModel::SearchConditionModel(QObject *parent)
    : QAbstractListModel(parent)
{
    reset();
}

int SearchConditionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return conditionList.size();
}

QVariant SearchConditionModel::data(const QModelIndex &index, int role) const
{
    //qDebug()<<__FUNCTION__<<index<<role;
    if (!index.isValid())
        return QVariant();
    const int row = index.row();
    const SearchCondition &item = conditionList.at(row);

    switch (role) {
    case C1MatchAllRole: return item.c1MatchAll;
    case C1KeywordsRole: return item.c1Keywords;
    case C2MatchAllRole: return item.c2MatchAll;
    case C2KeywordsRole: return item.c2Keywords;
    case EnableC2Role: return item.enableC2;
    case MatchAllRole: return item.matchAll;
    default: break;
    }

    return QVariant();
}

bool SearchConditionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //qDebug()<<__FUNCTION__<<index<<value<<role;
    if (!index.isValid() || value==data(index,role))
        return false;
    const int row = index.row();
    SearchCondition &item = conditionList[row];

    switch (role) {
    case C1MatchAllRole: item.c1MatchAll = value.toBool(); break;
    case C1KeywordsRole: item.c1Keywords = value.toString(); break;
    case C2MatchAllRole: item.c2MatchAll = value.toBool(); break;
    case C2KeywordsRole: item.c2Keywords = value.toString(); break;
        //case EnableC2Role: item.enableC2 = value.toBool(); break;
        //case MatchAllRole: item.matchAll = value.toBool(); break;
    default: return false;
    }

    //只是用ui上的值来设置model里的数据，不用刷新ui
    //emit dataChanged(index,index);
    return true;
}

QHash<int, QByteArray> SearchConditionModel::roleNames() const
{
    return QHash<int,QByteArray>{
        {C1MatchAllRole,"c1MatchAll"}
        ,{C1KeywordsRole,"c1Keywords"}
        ,{C2MatchAllRole,"c2MatchAll"}
        ,{C2KeywordsRole,"c2Keywords"}
        ,{EnableC2Role,"enableC2"}
        ,{MatchAllRole,"matchAll"}
    };
}

void SearchConditionModel::reset()
{
    //默认有一个搜索条件
    beginResetModel();
    conditionList.clear();
    SearchCondition one_condition;
    conditionList.push_back(one_condition);
    endResetModel();
}

void SearchConditionModel::enableCondition2(int row)
{
    if(!rowValid(row))
        return;
    SearchCondition &item = conditionList[row];
    item.c2MatchAll=false;
    item.c2Keywords.clear();
    item.enableC2=true; //开启条件2
    item.matchAll=false;
    emit dataChanged(index(row,0),index(row,0));
}

void SearchConditionModel::disableCondition2(int row)
{
    if(!rowValid(row))
        return;
    SearchCondition &item = conditionList[row];
    item.c2MatchAll=false;
    item.c2Keywords.clear();
    item.enableC2=false; //关闭条件2
    item.matchAll=false;
    emit dataChanged(index(row,0),index(row,0));
}

void SearchConditionModel::appendGroup()
{
    const int row = conditionList.size();
    beginInsertRows(QModelIndex(),row,row);
    SearchCondition one_condition;
    conditionList.push_back(one_condition);
    endInsertRows();
}

void SearchConditionModel::removeGroup(int row)
{
    if(!rowValid(row) || conditionList.size()<=1)
        return;
    beginRemoveRows(QModelIndex(),row,row);
    conditionList.takeAt(row);
    endRemoveRows();
}

void SearchConditionModel::saveCondition()
{
    qDebug()<<__FUNCTION__;
    cacheData.clear();
    QDataStream stream(&cacheData,QIODevice::WriteOnly);
    stream<<conditionList;
}

void SearchConditionModel::restoreCondition()
{
    qDebug()<<__FUNCTION__;
    if(cacheData.isEmpty()){
        qDebug()<<"cache is empty";
        return;
    }
    beginResetModel();
    conditionList.clear();
    QDataStream stream(&cacheData,QIODevice::ReadOnly);
    stream>>conditionList;
    endResetModel();
}

void SearchConditionModel::checkAndProcess()
{
    qDebug()<<__FUNCTION__;
    int error_row=-1;
    int error_col=-1;
    for(int i=0; i<conditionList.size(); i++)
    {
        const auto &item = conditionList.at(i);
        if(item.c1Keywords.isEmpty()){
            error_row=i+1;
            error_col=1;
            break;
        }else if(item.enableC2&&item.c2Keywords.isEmpty()){
            error_row=i+1;
            error_col=2;
            break;
        }
    }
    if(error_row!=-1){
        qDebug()<<"check error"<<error_row<<error_col;
    }
}

bool SearchConditionModel::rowValid(int row) const
{
    if(row<0 || row>=conditionList.size())
        return false;
    return true;
}
