#pragma once
#include <QAbstractListModel>

// 实际数据
struct DataInfo
{
    int value;
    // 本例用日期来分组
    QString date;
};

// 分组信息，如 index
struct SectionInfo
{
    int index;
};

class DataModel : public QAbstractListModel
{
    Q_OBJECT
private:
    enum ModelRole {
        ValueRole = Qt::UserRole
        , GroupNameRole
        , GroupIndexRole
    };
public:
    explicit DataModel(QObject *parent = nullptr);

    // Model 需要实现的必要接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 在头部添加一个数据
    Q_INVOKABLE void appendData(int value, const QString &date);
    // 根据 model.index 删除一个数据
    Q_INVOKABLE void removeData(int index);
    // 加点测试数据
    void test();

private:
    QVector<DataInfo> datas;
    QVector<SectionInfo> inners;
};
