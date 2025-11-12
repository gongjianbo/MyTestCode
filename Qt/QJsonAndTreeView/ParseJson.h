#pragma once

// JSON是一种存储结构化数据的格式，具有六种基本数据类型：
// bool、double、string、array、object、null
// QJsonDocument在core模块

// 参考：
// http://blog.sina.com.cn/s/blog_a6fb6cc90101gnxm.html
// https://www.cnblogs.com/lifan3a/articles/7811434.html
// https://doc.qt.io/qt-5/json.html

#include <QObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>

class ParseJson : public QObject
{
    Q_OBJECT
public:
    explicit ParseJson(QObject *parent = nullptr);
    // 加载Json文件
    bool loadJson(const QString &filepath);
    // 导出Json文件
    bool dumpJson(const QString &filepath);

private:
    // 加载Json时解析Object
    void parseObject(const QJsonObject &obj);
    // 加载Json时解析Array
    void parseArray(const QJsonArray &arr);
    // 加载Json时解析Value
    void parseValue(const QJsonValue &val);

};
