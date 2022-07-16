#pragma once
#include <QAbstractListModel>
#include "DataSource.h"

/**
 * @brief 数据model
 * @author 龚建波
 * @date 2022-07-10
 */
class DataModel : public QAbstractListModel
{
    Q_OBJECT
private:
    enum ModelRole
    {
        KeyRole = Qt::UserRole
        ,ValueRole
    };
public:
    explicit DataModel(QObject *parent = nullptr);

    //basic interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

    //增加一个item
    Q_INVOKABLE void appendItem(const QString &key, int value);
    //删除一个item
    Q_INVOKABLE void deleteItemByRow(int row);
    //修改一个item
    Q_INVOKABLE void updateItemByRow(int row, const QString &key, int value);
    //根据value范围查询一组item
    Q_INVOKABLE void searchItemByValue(int min, int max);

private:
    //展示的数据
    QList<QSharedPointer<DataInfo>> dataList;
    //缓存搜索条件
    int searchMin = -1;
    int searchMax = -1;
};
