#pragma once
#include <QStringListModel>
#include <QSortFilterProxyModel>

class CompleterProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSortFilterProxyModel* filterModel READ getFilterModel CONSTANT)
    Q_PROPERTY(QStringList keywords READ getKeywords WRITE setKeywords NOTIFY keywordsChanged)
public:
    explicit CompleterProxy(QObject *parent = nullptr);

    //设置给ui列表的model
    QSortFilterProxyModel *getFilterModel();

    //用于提示的关键字列表
    QStringList getKeywords() const;
    void setKeywords(const QStringList &keys);

signals:
    void keywordsChanged();

private:
    QStringListModel model;
    QSortFilterProxyModel filter;
};
