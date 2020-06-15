#ifndef MYMAPPER_H
#define MYMAPPER_H

#include <QObject>
#include <QWidget>
#include <QStandardItemModel>

//表示一个绑定关系的结构体
struct MapperItem
{
    //widget通过属性获取和设置值
    QWidget *widget;
    QByteArray property;
    //model通过column和role获取和设置值
    int column;
};

//自定义mapper，手动设置和获取值
class MyMapper : public QObject
{
    Q_OBJECT
public:
    explicit MyMapper(QObject *parent = nullptr);

    //设置数据model
    void setModel(QStandardItemModel *model);
    //设置映射的widget
    void addMapping(QWidget *w, int column, const QByteArray &property);
    //设置model当前映射的row
    void setRow(int row);
    void toFirst();
    void toNext();
    void toPrev();
    //更改model
    void submit();

private:
    //mapper列表
    QList<MapperItem> mapperList;
    //model
    int currentRow=-1;
    QStandardItemModel *model=nullptr;
};

#endif // MYMAPPER_H
