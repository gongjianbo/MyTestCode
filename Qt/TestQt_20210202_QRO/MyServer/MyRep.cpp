#include "MyRep.h"

#include <QDebug>

MyRep::MyRep(QObject *parent)
    : MyRepSource(parent)
{

}

void MyRep::myRecv(const QString &data, int num)
{
    qDebug()<<"recv"<<data<<num;

    emit mySend(data,num+1000);
}
