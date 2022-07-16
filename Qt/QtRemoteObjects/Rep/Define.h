#pragma once
#include <QObject>
#include <QDataStream>

//支持使用自定义结构体，但是只能以include的方式，不能直接在rep中定义
//由于自动生成的代码需要比较和序列化，所以要重载一些函数
struct MyStruct
{
    QString name;
    int age;
    bool sex;
};
Q_DECLARE_METATYPE(MyStruct)

inline bool operator==(const MyStruct &left, const MyStruct &right)
{
    return left.name == right.name && left.age == right.age && left.sex == right.sex;
}

inline bool operator!=(const MyStruct &left, const MyStruct &right)
{
    return !(left == right);
}

//重载输入输出运算符，以支持datastream序列化自定义结构
inline QDataStream& operator <<(QDataStream& out, const MyStruct& item)
{
    out<<item.name<<item.age<<item.sex;
    return out;
}

inline QDataStream& operator >>(QDataStream& in, MyStruct& item)
{
    in>>item.name>>item.age>>item.sex;
    return in;
}
