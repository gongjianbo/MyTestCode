#include "DataModel.h"

DataModel::DataModel(QObject *parent)
    : QAbstractListModel(parent)
{
    test();
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return datas.size();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto &&item = datas.at(index.row());
    auto &&inner = inners.at(index.row());
    switch (role)
    {
    case ValueRole: return item.value;
    case GroupNameRole: return item.date;
    case GroupIndexRole: return inner.index;
    }
    return QVariant();
}

QHash<int, QByteArray> DataModel::roleNames() const
{
    static QHash<int, QByteArray> names{
        {ValueRole, "value"}
        , {GroupNameRole, "groupName"}
        , {GroupIndexRole, "groupIndex"}
    };
    return names;
}

void DataModel::appendData(int value, const QString &date)
{
    // 先判断分组是否相同
    if (datas.isEmpty() || datas.first().date != date) {
        // 没有该组，新建一个分组
        DataInfo item;
        item.value = value;
        item.date = date;
        SectionInfo inner;
        inner.index = 0;
        beginInsertRows(QModelIndex(), 0, 0);
        datas.push_front(item);
        inners.push_front(inner);
        endInsertRows();
    } else {
        // 已有该组，插入并移动该组后面的 Item
        DataInfo item;
        item.value = value;
        item.date = date;
        SectionInfo inner;
        inner.index = 0;
        // 这里本来应该insert到0，但是在Qt5.15实测有bug
        // 如果顶部不可见再insert到0，会新增一个section，原来的section没有移动到最前
        // 所以取巧insert到1，然后再把这个分组整体刷新一下
        beginInsertRows(QModelIndex(), 1, 1);
        datas.push_front(item);
        inners.push_front(inner);
        endInsertRows();

        // 刷新该组index
        int last_index = 0;
        // 0 是新插入，1 是旧 0
        for (int i = 1; i < inners.size(); i++) {
            auto &&inner_i = inners[i];
            if (i > 1 && inner_i.index == 0)
                break;
            inner_i.index = i;
            last_index++;
        }
        // 没更新数据的可以置顶具体role刷新
        emit dataChanged(QAbstractListModel::index(0, 0), QAbstractListModel::index(last_index, 0));
    }
}

void DataModel::removeData(int index)
{
    if (index < 0 || index >= datas.size())
        return;
    beginRemoveRows(QModelIndex(), index, index);
    datas.removeAt(index);
    inners.removeAt(index);
    endRemoveRows();
    int update_count = 0;
    for (int i = index; i < inners.size(); i++) {
        auto &&inner_i = inners[i];
        if (inner_i.index == 0)
            break;
        inner_i.index -= 1;
        update_count ++;
    }
    if (update_count > 0) {
        emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index + update_count, 0));
    }
}

void DataModel::test()
{
    DataInfo item;
    SectionInfo inner;
    item.date = "2022.2.22";
    for (int i = 0; i < 3; i++)
    {
        item.value = i + 1;
        datas.push_back(item);
        inner.index = i;
        inners.push_back(inner);
    }
    item.date = "2010.10.10";
    for (int i = 0; i < 21; i++)
    {
        item.value = i + 1;
        datas.push_back(item);
        inner.index = i;
        inners.push_back(inner);
    }
    item.date = "1999.9.9";
    for (int i = 0; i < 31; i++)
    {
        item.value = i + 1;
        datas.push_back(item);
        inner.index = i;
        inners.push_back(inner);
    }
}
