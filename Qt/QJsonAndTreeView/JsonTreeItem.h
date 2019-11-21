#ifndef JSONTREEITEM_H
#define JSONTREEITEM_H

#include <QList>

//对于多种结构，可以写一个item抽象父类
//我这个Item比较简单，基本都是靠外部来设置
class JsonTreeItem
{
public:
    //item的类型，None为无效的类型
    enum JsonItemType{
        None,
        Object,
        Array,
        Value
    };
public:
    explicit JsonTreeItem(JsonTreeItem *parent=nullptr);
    explicit JsonTreeItem(const QHash<int,QVariant> &datas,JsonTreeItem::JsonItemType type,JsonTreeItem *parent=nullptr);
    ~JsonTreeItem();

    //增删操作
    bool insertChild(int row,JsonTreeItem *child);
    bool removeChild(int row);
    bool insertChildren(int row,int count);
    bool removeChildren(int row,int count);
    void appendChild(JsonTreeItem *child);
    void deleteAllChild();

    //用于Model结构
    JsonTreeItem *childItem(int row);
    JsonTreeItem *parentItem();
    void setParentItem(JsonTreeItem *parent);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void setData(int column,const QVariant &val);
    int row() const;
    bool editable(int column) const;
    //QHash<int,QByteArray> roleNames() const;

    //便于Json操作
    QString key() const;
    void setKey(const QString &key);
    QVariant value() const;
    void setValue(const QVariant &value);
    JsonTreeItem::JsonItemType type() const;
    void setType(JsonTreeItem::JsonItemType type);

private:
    JsonTreeItem *theParentItem;         //构造函数来初始化parent
    QList<JsonTreeItem*> theChildItems;  //append来添加child
    JsonItemType theItemType;
    //hash内容
    //1：key
    //2：value
    //用hash是为了便于扩展，比如增加勾选列等
    //如果只是简单的key+value两行数据，就用QString+QVariant就行了
    QHash<int,QVariant> theItemDatas;    //存放每列（角色）的内容（qml的tree使用角色）
};

#endif // JSONTREEITEM_H
