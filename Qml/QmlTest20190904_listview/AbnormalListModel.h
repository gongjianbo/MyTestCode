#ifndef ABNORMALLISTMODEL_H
#define ABNORMALLISTMODEL_H

#include <array>
#include <QAbstractListModel>
#include <QList>
#include <QTime>


class AbnormalListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Roles)
    Q_PROPERTY(int currentIndex READ getCurrentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

    static constexpr int rowLimit=24;
    static constexpr int colLimit=60;
    using RowData=std::array<int,colLimit>; //int为信号异常编号
public:
    enum Roles{
        HourRole=Qt::UserRole+1,
    };

    struct TableData
    {
        int freq;
        QList<QTime> tableHour;
        QList<RowData> tableRows;
    };
    explicit AbnormalListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int,QByteArray> roleNames() const override{
        return QHash<int,QByteArray>{
            {HourRole,"hour"}
        };
    }
    //
    int getCurrentIndex() const;
    void setCurrentIndex(int index);

    //自定义data类或者每个role都定义会写很长，直接用get方法获取
    Q_INVOKABLE int abnormal(int row,int col) const;
    Q_INVOKABLE QStringList groupList() const;

private:
    bool indexValid(int index,int row,int col) const;

signals:
    void currentIndexChanged();

private:
    QList<TableData> modelData;
    int currentIndex=-1;
};

#endif // ABNORMALLISTMODEL_H
