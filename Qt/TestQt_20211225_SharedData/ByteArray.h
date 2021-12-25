#pragma once
//参照Qt文档：https://doc.qt.io/qt-5/qshareddatapointer.html
#include <QSharedData>
#include <QSharedDataPointer>
#include <QDebug>

//共享的数据
//QSharedData带有原子类型的引用计数
class ByteData : public QSharedData
{
public:
    ByteData() {
        qDebug()<<"ByteData()";
    }
    //拷贝构造不是必要的
    ByteData(const ByteData &other)
        : QSharedData(other),
          num(other.num),
          name(other.name) {
        qDebug()<<"ByteData(const ByteData &other)";
    }
    ~ByteData() {
        qDebug()<<"~ByteData()";
    }

    int getNum() const { return num; }
    void setNum(int i) { num = i; }
    QString getName() const { return name; }
    void setName(const QString &str) { name = str; }

private:
    int num{0};
    QString name;
};

//隐式共享类
//修改数据时调用QSharedDataPointer的detach创建副本
class ByteArray
{
public:
    ByteArray() { d = new ByteData; }
    ByteArray(const ByteArray &other) : d(other.d) { }

    //非const接口会自动创建副本，而const接口与其他共享引用
    int getNum() const { return d->getNum(); }
    void setNum(int i) { d->setNum(i); }
    QString getName() const { return d->getName(); }
    void setName(const QString &str) { d->setName(str); }

private:
    QSharedDataPointer<ByteData> d;
};

