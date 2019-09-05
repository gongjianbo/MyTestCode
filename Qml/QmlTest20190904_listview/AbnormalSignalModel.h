#ifndef ABNORMALSIGNALMODEL_H
#define ABNORMALSIGNALMODEL_H

#include <array>
#include <QAbstractTableModel>
#include <QList>
#include <QPoint>
#include <QTime>

//table以替换为list
class AbnormalSignalModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Roles)
public:
    enum Roles{
        ItemRole, //单元格
        VHeaderRole, //竖向表头
        HHeaderRole, //横向表头
        OtherRole //其他，如横竖夹角
    };
    struct SignalInfo
    {
        int abnormal=0;
        int freq;
    };
    explicit AbnormalSignalModel(QObject *parent = nullptr);

    // Header:
    //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int,QByteArray> roleNames() const override{
        return QHash<int,QByteArray>{
            {ItemRole,"item"},
            {VHeaderRole,"vheader"},
            {HHeaderRole,"hheader"},
            {OtherRole,"other"}
        };
    }
private:
    static constexpr int cols=60;
    static constexpr int rows=24;
    //static constexpr int width=col+1;
    //static constexpr int height=row+1;

    using RowArray=std::array<SignalInfo,cols>;

    QList<RowArray> modelItems;
    QList<QTime> modelVHeader;
};

#endif // ABNORMALSIGNALMODEL_H
