#ifndef CPPOBJECT_H
#define CPPOBJECT_H

#include <QObject>
#include <QDebug>

class CppObject : public QObject
{
    Q_OBJECT
public:
    explicit CppObject(QObject *parent = Q_NULLPTR)
        :QObject(parent){}

signals:
    //信号 --用来触发qml的函数
    //注意参数为var类型，对应qml中js函数的参数类型
    void cppSendMsg(const QVariant &arg1,const QVariant &arg2);

public slots:
    //槽函数 --用来接收qml的信号
    void cppRecvMsg(const QString &arg1,const QString &arg2){
        qDebug()<<"CppObject::cppRecvMsg"<<arg1<<arg2;
        qDebug()<<"emit cppSendMsg";
        emit cppSendMsg(arg1,arg2);
    }
};

#endif // CPPOBJECT_H
