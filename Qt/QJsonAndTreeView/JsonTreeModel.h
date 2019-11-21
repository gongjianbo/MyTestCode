#ifndef JSONTREEMODEL_H
#define JSONTREEMODEL_H

#include <QAbstractItemModel>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>

#include "JsonTreeItem.h"

class JsonTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit JsonTreeModel(QObject *parent = nullptr);
    ~JsonTreeModel();

    //导入导出数据
    bool loadJson(const QString &filepath);
    bool dumpJson(const QString &filepath);
    
    //tree数据展示
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //支持编辑
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;

    //QHash<int,QByteArray> roleNames() const override; //qml中需要，widget不用
    
private:
    JsonTreeItem *getItem(const QModelIndex &index) const;
    //解析并生成tree
    void parseObject(const QString &key,const QJsonObject& obj,JsonTreeItem *&item);
    void parseArray(const QString &key,const QJsonArray& arr,JsonTreeItem *&item);
    void parseValue(const QString &key,const QJsonValue& val,JsonTreeItem *&item);
    //生成json节点
    QVariantMap dumpObject(JsonTreeItem *&item) const;
    QVariantList dumpArray(JsonTreeItem *&item) const;
    QVariant dumpValue(JsonTreeItem *&item) const;
    
private:
    JsonTreeItem *theRootItem;
};

#endif // JSONTREEMODEL_H
