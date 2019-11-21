#include "JsonTreeItem.h"

JsonTreeItem::JsonTreeItem(JsonTreeItem *parent)
    :theParentItem(parent)
    ,theItemType(JsonTreeItem::None)
    ,theItemDatas({{0,"[Key]"},{1,"[Value]"}}) //默认两行
{    
}

JsonTreeItem::JsonTreeItem(const QHash<int, QVariant> &datas, JsonTreeItem::JsonItemType type, JsonTreeItem *parent)
    :theParentItem(parent)
    ,theItemType(type)
    ,theItemDatas(datas)
{
}

JsonTreeItem::~JsonTreeItem()
{
    deleteAllChild();
}

bool JsonTreeItem::insertChild(int row, JsonTreeItem *child)
{
    if(row<0||row>theChildItems.count())
        return false;
    theChildItems.insert(row,child);
    child->setParentItem(this);
    return true;
}

bool JsonTreeItem::removeChild(int row)
{
    if(row<0||row+1>theChildItems.count())
        return false;
    delete theChildItems.takeAt(row);
    return true;
}

bool JsonTreeItem::insertChildren(int row, int count)
{
    if(row<0||row>theChildItems.count())
        return false;
    for(int i=0;i<count;i++){
        JsonTreeItem *item=new JsonTreeItem(this);
        //目前还没有delegate来自定义操作，所以new item默认为json Value类型
        item->setType(JsonTreeItem::Value);
        //这里新增的次序无所谓row+i
        theChildItems.insert(row,item);
    }
    return true;
}

bool JsonTreeItem::removeChildren(int row, int count)
{
    if (row<0||row+count>theChildItems.count())
        return false;
    for(int i=0;i<count;i++){
        delete theChildItems.takeAt(row+i);
    }
    return true;
}

void JsonTreeItem::appendChild(JsonTreeItem *child)
{
    theChildItems.append(child);
    child->setParentItem(this);
}

void JsonTreeItem::deleteAllChild()
{
    qDeleteAll(theChildItems);
    theChildItems.clear();
}

JsonTreeItem *JsonTreeItem::childItem(int row)
{
    return theChildItems.value(row);
}

JsonTreeItem *JsonTreeItem::parentItem()
{
    return theParentItem;
}

void JsonTreeItem::setParentItem(JsonTreeItem *parent)
{
    theParentItem=parent;
}

int JsonTreeItem::childCount() const
{
    return theChildItems.count();
}

int JsonTreeItem::columnCount() const
{
    return theItemDatas.count();
}

QVariant JsonTreeItem::data(int column) const
{
    return theItemDatas.value(column,QVariant());
}

void JsonTreeItem::setData(int column, const QVariant &val)
{
    theItemDatas.insert(column,val);
}

int JsonTreeItem::row() const
{
    if(theParentItem)
        return theParentItem->theChildItems.indexOf(const_cast<JsonTreeItem*>(this));
    return 0;
}

bool JsonTreeItem::editable(int column) const
{
    //在保留默认结构的情况下
    //root的key-value都不可编辑
    //-->parent:nullptr
    //array下级的key不可编辑
    //-->parent:array
    //array或object的value不可编辑
    //-->type()
    //此外，如果希望key不可变，column==0返回false即可
    if((!theParentItem||!theParentItem->parentItem())||
            ((0==column)&&(theParentItem->type()==JsonTreeItem::Array))||
            ((1==column)&&((type()==JsonTreeItem::Array)||(type()==JsonTreeItem::Object))))
        return false;
    return true;
}

QString JsonTreeItem::key() const
{
    return theItemDatas.value(0,"").toString();
}

void JsonTreeItem::setKey(const QString &key)
{
    theItemDatas[0]=key;
}

QVariant JsonTreeItem::value() const
{
    return theItemDatas.value(1,0);
}

void JsonTreeItem::setValue(const QVariant &value)
{
    theItemDatas[1]=value;
}

JsonTreeItem::JsonItemType JsonTreeItem::type() const
{
    return theItemType;
}

void JsonTreeItem::setType(JsonTreeItem::JsonItemType type)
{
    theItemType=type;
}

