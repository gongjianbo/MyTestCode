#include "MySource.h"

void MySource::setData(const QString &data)
{
    myData = data;
    emit dataChanged(data);
}

QString MySource::getData()
{
    return myData;
}
