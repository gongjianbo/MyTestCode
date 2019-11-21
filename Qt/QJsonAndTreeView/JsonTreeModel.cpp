#include "JsonTreeModel.h"

#include <QFile>
#include <QDebug>

JsonTreeModel::JsonTreeModel(QObject *parent)
    : QAbstractItemModel(parent),theRootItem(new JsonTreeItem(nullptr))
{

}

JsonTreeModel::~JsonTreeModel()
{
    delete theRootItem;
}

bool JsonTreeModel::loadJson(const QString &filepath)
{
    //判断路径以及是否正常打开
    if(filepath.isEmpty())
        return false;
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;

    //读取数据后关闭文件
    const QByteArray raw_data=file.readAll();
    file.close();

    //解析为Json文档
    QJsonParseError json_error;
    QJsonDocument json_doc=QJsonDocument::fromJson(raw_data,&json_error);

    //是否正常解析Json数据
    if(json_doc.isNull()||json_doc.isEmpty()||json_error.error!=QJsonParseError::NoError)
        return false;

    emit beginResetModel();
    theRootItem->deleteAllChild();//清空之前的model

    //判断是object{}还是array[]的Json
    if(json_doc.isObject()){
        //解析文档中的Object
        parseObject("[Root]",json_doc.object(),theRootItem);
    }else if(json_doc.isArray()){
        //解析文档中的Array
        parseArray("[Root]",json_doc.array(),theRootItem);
    }
    emit endResetModel();

    qDebug()<<"load json file";
    return true;
}

bool JsonTreeModel::dumpJson(const QString &filepath)
{
    if(filepath.isEmpty())
        return false;

    //root 的child最多也只有一个
    if(!theRootItem||theRootItem->childCount()==0)
        return false;
    JsonTreeItem *top_level_item=theRootItem->childItem(0);
    if(!top_level_item)
        return false;

    //遍历Tree生成JsonDocument
    QJsonDocument json_doc;
    switch (top_level_item->type()) {
    case JsonTreeItem::Object:
        json_doc=QJsonDocument::fromVariant(dumpObject(top_level_item));
        break;
    case JsonTreeItem::Array:
        json_doc=QJsonDocument::fromVariant(dumpArray(top_level_item));
        break;
    default:
        break;
    }

    //导出Json
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    //转换为bytearray，Compact没有换行，Indented有换行可读性更强
    file.write(json_doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug()<<"dump json file";
    return true;
}

QModelIndex JsonTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    JsonTreeItem *parentItem=getItem(parent);
    JsonTreeItem *childItem = parentItem->childItem(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex JsonTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    JsonTreeItem *childItem = getItem(index);
    JsonTreeItem *parentItem = childItem->parentItem();

    if (parentItem == theRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int JsonTreeModel::rowCount(const QModelIndex &parent) const
{
    JsonTreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

int JsonTreeModel::columnCount(const QModelIndex &parent) const
{
    JsonTreeItem *parentItem = getItem(parent);
    return parentItem->columnCount();
}

QVariant JsonTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    JsonTreeItem *item = getItem(index);
    return item->data(index.column());
}

Qt::ItemFlags JsonTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    JsonTreeItem *item = getItem(index);
    return (item->editable(index.column())?Qt::ItemIsEditable:Qt::NoItemFlags)
            |QAbstractItemModel::flags(index);
}

bool JsonTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    JsonTreeItem *item = getItem(index);
    item->setData(index.column(), value);
    emit dataChanged(index, index, {role});

    return true;
}

bool JsonTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    JsonTreeItem *parentItem=getItem(parent);

    beginInsertRows(parent, row, row+count-1);
    const bool result=parentItem->insertChildren(row,count);
    endInsertRows();

    return result;
}

bool JsonTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    JsonTreeItem *parentItem=getItem(parent);

    beginRemoveRows(parent, row, row+count-1);
    const bool result=parentItem->removeChildren(row,count);
    endRemoveRows();

    return result;
}

JsonTreeItem *JsonTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        JsonTreeItem *item = static_cast<JsonTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return theRootItem;
}

void JsonTreeModel::parseObject(const QString &key, const QJsonObject &obj, JsonTreeItem *&item)
{
    //构造Object节点
    JsonTreeItem *child=new JsonTreeItem({{0,key},{1,"[Object]"}},JsonTreeItem::Object,item);
    item->appendChild(child);

    //遍历Object的keys
    const QStringList keys=obj.keys();
    for(const QString &item_key:keys){
        //qDebug()<<"key:"<<item_key;
        parseValue(item_key,obj.value(item_key),child);
    }
}

void JsonTreeModel::parseArray(const QString &key, const QJsonArray &arr, JsonTreeItem *&item)
{
    //构造Array节点
    JsonTreeItem *child=new JsonTreeItem({{0,key},{1,"[Array]"}},JsonTreeItem::Array,item);
    item->appendChild(child);

    //遍历Array
    for(int i=0;i<arr.count();i++){
        parseValue("-",arr.at(i),child);
    }
}

void JsonTreeModel::parseValue(const QString &key, const QJsonValue &val, JsonTreeItem *&item)
{
    QVariant the_val;
    //根据Value的类型进行下一步处理
    //如果是Object或Array就继续递归
    //如果是值就添加一个节点
    switch (val.type()) {
    case QJsonValue::Object:
        parseObject(key,val.toObject(),item);
        return;
        break;
    case QJsonValue::Array:
        parseArray(key,val.toArray(),item);
        return;
        break;
    case QJsonValue::Bool:
        the_val=val.toBool();
        break;
    case QJsonValue::Double:
        the_val=val.toDouble();
        break;
    case QJsonValue::String:
        the_val=val.toString();
        break;
    case QJsonValue::Null: break;
    case QJsonValue::Undefined: break;
    default: break;
    }

    //构造子节点
    JsonTreeItem *child=new JsonTreeItem({{0,key},{1,the_val}},JsonTreeItem::Value,item);
    item->appendChild(child);
}

QVariantMap JsonTreeModel::dumpObject(JsonTreeItem *&item) const
{
    QVariantMap json_obj; //QVariantMap对应QJsonObject
    const int child_count=item->childCount();
    for(int i=0;i<child_count;i++){
        JsonTreeItem *child=item->childItem(i);
        if(!child) continue;
        //为什么不用一个返回QVariant的函数之类的封装下？
        switch (child->type()) {
        case JsonTreeItem::Object:
            json_obj.insert(child->key(),dumpObject(child));
            break;
        case JsonTreeItem::Array:
            json_obj.insert(child->key(),dumpArray(child));
            break;
        case JsonTreeItem::Value:
            json_obj.insert(child->key(),dumpValue(child));
            break;
        default:
            break;
        }
    }
    return json_obj;
}

QVariantList JsonTreeModel::dumpArray(JsonTreeItem *&item) const
{
    QVariantList json_arr; //QVariantList对应QJsonArray
    const int child_count=item->childCount();
    for(int i=0;i<child_count;i++){
        JsonTreeItem *child=item->childItem(i);
        if(!child) continue;
        switch (child->type()) {
        case JsonTreeItem::Object:
            json_arr.append(dumpObject(child));
            break;
        case JsonTreeItem::Array:
            json_arr.append(dumpArray(child));
            break;
        case JsonTreeItem::Value:
            json_arr.append(dumpValue(child));
            break;
        default:
            break;
        }
    }
    return json_arr;
}

QVariant JsonTreeModel::dumpValue(JsonTreeItem *&item) const
{
    //QVariant对应QJsonValue
    return item->value();
}

