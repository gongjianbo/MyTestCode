#pragma once
#include <QObject>

//访问剪切板，这个操作可以放到一个工具类里
class Clipboard : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    //ip放到剪切板
    Q_INVOKABLE void copyIp(QStringList ip);
    //读取剪切板ip
    Q_INVOKABLE QStringList pasteIp();
};

