#pragma once
#include <QObject>
#include <QList>
#include <QSharedPointer>

/**
 * @brief 数据信息
 * @author 龚建波
 * @date 2022-07-10
 */
struct DataInfo
{
    DataInfo(const QString &str, int val);
    ~DataInfo();

    QString key;
    int value = 0;

    //调试用，记录构造和释放顺序
    static int counter;
    int id = 0;
};

/**
 * @brief 管理共享的数据
 * @author 龚建波
 * @date 2022-07-10
 */
class DataSource : public QObject
{
    Q_OBJECT
private:
    explicit DataSource(QObject *parent = nullptr);
public:
    ~DataSource();
    static DataSource* getInstance();

    //增删改接口
    void deleteItem(QSharedPointer<DataInfo> item);
    void appendItem(const QString &key, int value);
    void appendItem(QSharedPointer<DataInfo> item);
    void updateItem(QSharedPointer<DataInfo> item);
    //根据value范围查询数据
    QList<QSharedPointer<DataInfo>> searchItemByValue(int min, int max);

signals:
    //增删改通知其他view更新
    void itemDeleteNotify(QSharedPointer<DataInfo> item);
    void itemAppendNotify(QSharedPointer<DataInfo> item);
    void itemUpdateNotify(QSharedPointer<DataInfo> item);

private:
    //存储着所有的数据
    QList<QSharedPointer<DataInfo>> dataList;
};
