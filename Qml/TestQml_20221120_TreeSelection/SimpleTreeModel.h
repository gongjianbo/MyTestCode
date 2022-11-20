#pragma once
#include <QStandardItemModel>
#include <QStandardItem>

class SimpleTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit SimpleTreeModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //添加一个节点
    Q_INVOKABLE void appendNode(const QModelIndex &index, const QString &text);
    //删除一个节点
    Q_INVOKABLE void removeNode(const QModelIndex &index);
    //重置
    Q_INVOKABLE void reset();
    //清除
    Q_INVOKABLE void clear();
};
