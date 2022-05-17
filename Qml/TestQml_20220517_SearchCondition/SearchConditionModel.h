#pragma once
#include <QAbstractListModel>
#include <QDataStream>

//一组过滤条件
struct SearchCondition
{
    //条件1
    bool c1MatchAll=false; //=true同时满足，=false满足一个
    QString c1Keywords; //搜索条件关键词，分号分隔
    //条件2
    bool c2MatchAll=false;
    QString c2Keywords;
    //分组设置
    bool enableC2=false; //=true开启条件2，默认false
    bool matchAll=false; //=true两个条件同时满足，=false满足一个
};
QDataStream& operator<<(QDataStream& out, const SearchCondition& item);
QDataStream& operator>>(QDataStream& in, SearchCondition& item);

class SearchConditionModel : public QAbstractListModel
{
    Q_OBJECT
private:
    enum ModelRole
    {
        C1MatchAllRole=Qt::UserRole
        ,C1KeywordsRole
        ,C2MatchAllRole
        ,C2KeywordsRole
        ,EnableC2Role
        ,MatchAllRole
    };
public:
    explicit SearchConditionModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int,QByteArray> roleNames() const override;

    //清空，重置为默认搜索条件
    Q_INVOKABLE void reset();
    //分组条件2使能
    Q_INVOKABLE void enableCondition2(int row);
    Q_INVOKABLE void disableCondition2(int row);
    //新增一组条件
    Q_INVOKABLE void appendGroup();
    //删除一组条件
    Q_INVOKABLE void removeGroup(int row);

    //保存记录
    Q_INVOKABLE void saveCondition();
    //恢复记录
    Q_INVOKABLE void restoreCondition();
    //开始处理，校验是否有没填写的条件
    Q_INVOKABLE void checkAndProcess();

private:
    bool rowValid(int row) const;

private:
    //分组条件
    QList<SearchCondition> conditionList;
    //测试保存/恢复记录
    QByteArray cacheData;
};
