#include "CppObject.h"

#include <QDebug>

CppObject::CppObject(QObject *parent)
    : QObject(parent),
      myName("none"),
      myYear(0)
{

}

void CppObject::sendSignal()
{
    //测试用，调用该函数后发送信号
    qDebug()<<"CppObject::sendSignal";
    emit cppSignalA();
    emit cppSignalB(myName,myYear);
}

void CppObject::setName(const QString &name)
{
    qDebug()<<"CppObject::setName"<<name;
    if(myName!=name){
        qDebug()<<"emit nameChanged";
        myName=name;
        emit nameChanged(name);
    }
}

QString CppObject::getName() const
{
    qDebug()<<"CppObject::getName";
    return myName;
}

void CppObject::setYear(int year)
{
    qDebug()<<"CppObject::setYear"<<year;
    if(year!=myYear){
        qDebug()<<"emit yearChanged";
        myYear=year;
        emit yearChanged(myYear);
    }
}

int CppObject::getYear() const
{
    qDebug()<<"CppObject::getYear";
    return myYear;
}

void CppObject::cppSlotA()
{
    qDebug()<<"CppObject::cppSlotA";
}

void CppObject::cppSlotB(const QString &str, int value)
{
    qDebug()<<"CppObject::cppSlotB"<<str<<value;
}
