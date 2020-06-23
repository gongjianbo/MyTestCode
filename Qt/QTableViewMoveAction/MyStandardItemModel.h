#ifndef MYSTANDARDITEMMODEL_H
#define MYSTANDARDITEMMODEL_H

#include <QStandardItem>
#include <QStandardItemModel>

class MyStandardItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit MyStandardItemModel(QObject *parent = nullptr);

    // 允许的操作，加上drag drop
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    // 允许move
    Qt::DropActions supportedDropActions() const override;
    // drag时携带的信息
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    // drop时根据drag携带的信息进行处理
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};

#endif // MYSTANDARDITEMMODEL_H
