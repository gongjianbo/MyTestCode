#include "ListModel.h"
#include "Translator.h"

ListModel::ListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    modelData = QStringList{QObject::tr("第一项"),QObject::tr("第二项"),QObject::tr("第三项")};
    //connect(Translator::getInstance(), &Translator::languageChanged, this,[this]{
    //    //emit dataChanged(index(0,0), index(rowCount()-1,0));
    //    beginResetModel();
    //    modelData = QStringList{QObject::tr("第一项"),QObject::tr("第二项"),QObject::tr("第三项")};
    //    endResetModel();
    //});
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return modelData.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::UserRole){
        return modelData.at(index.row());
    }else if(role == Qt::UserRole+1){
        return tr("翻译");
    }

    return QVariant();
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {Qt::UserRole, "txt"},
        {Qt::UserRole+1, "trans"}
    };
}
