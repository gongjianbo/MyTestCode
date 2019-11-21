#include "ParseJson.h"

#include <QFile>
#include <QDebug>

ParseJson::ParseJson(QObject *parent)
    :QObject(parent)
{

}

bool ParseJson::loadJson(const QString &filepath)
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

    //判断是object{}还是array[]的Json
    if(json_doc.isObject()){
        //解析文档中的Object
        parseObject(json_doc.object());
    }else if(json_doc.isArray()){
        //解析文档中的Array
        parseArray(json_doc.array());
    }

    //如果是读取指定key的数据，那么先判断Object是否contains这个key
    //    const QJsonObject root_obj=json_doc.object();
    //    if(root_obj.contains("name")){
    //        const QJsonValue json_value=root_obj.value("name");
    //        //如果用root_obj.take("name"),那么原对象中的这一项会被删除。
    //        if(json_value.isString()){
    //            qDebug()<<"name:"<<json_value.toString();
    //        }
    //    }

    qDebug()<<"load json end:"<<filepath;

    return true;
}

bool ParseJson::dumpJson(const QString &filepath)
{
    //判断路径
    if(filepath.isEmpty())
        return false;

    //这里构造一个Json，并添加模拟的数据
    QJsonDocument json_doc;

    QJsonArray json_arr;
    json_arr.insert(0,"Gong");
    json_arr.insert(1,"Jian");
    json_arr.insert(2,"Bo");
    json_arr.append("1992");

    QJsonObject json_obj;
    json_obj.insert("name","Qt");
    QJsonObject json_subobj{{"major",5},{"minor",12},{"patch",6}};
    json_obj.insert("version",json_subobj);
    json_obj.insert("array",json_arr);
    json_doc.setObject(json_obj);

    //转换为bytearray，Compact没有换行，Indented有换行可读性更强
    QByteArray json_data=json_doc.toJson(QJsonDocument::Indented);

    //判断是否正常打开
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly))
        return false;
    file.write(json_data);
    //File好像没实现waitForBytesWritten
    //const bool result=file.waitForBytesWritten(0);
    //qDebug()<<file.error();
    file.close();

    qDebug()<<"dump json end:"<<filepath;

    return true;
}

void ParseJson::parseObject(const QJsonObject &obj)
{
    qDebug()<<"object:";
    //获取Object中的keys，然后遍历
    const QStringList keys=obj.keys();
    for(const QString &item_key:keys){
        qDebug()<<"key:"<<item_key;
        //这里注意，如果是value函数，则返回的QJsonValue
        //如果是[]访问，有两种情况，const函数中返回的是QJsonValue
        //QJsonValue operator[](const QString &key) const
        //QJsonValueRef	operator[](const QString &key)
        //QJsonValueRef是QJsonArray和QJsonObject的帮助器类
        parseValue(obj.value(item_key));
    }
}

void ParseJson::parseArray(const QJsonArray &arr)
{
    qDebug()<<"array:";
    for(int i=0;i<arr.count();i++){
        parseValue(arr.at(i));
    }
}

void ParseJson::parseValue(const QJsonValue &val)
{
    qDebug()<<"value:";
    QVariant item_val;
    switch (val.type()) {
    case QJsonValue::Object:
        //如果是Object继续递归解析
        parseObject(val.toObject());
        return;
        break;
    case QJsonValue::Array:
        //如果是Array继续递归解析
        parseArray(val.toArray());
        return;
        break;
    case QJsonValue::Bool:
        item_val=val.toBool();
        break;
    case QJsonValue::Double:
        item_val=val.toDouble();
        break;
    case QJsonValue::String:
        item_val=val.toString();
        break;
    case QJsonValue::Null: break;
    case QJsonValue::Undefined: break;
    default: break;
    }

    qDebug()<<item_val;
}
