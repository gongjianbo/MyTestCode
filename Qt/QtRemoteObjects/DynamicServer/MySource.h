#pragma once
#include <QObject>

class MySource : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_INVOKABLE void setData(const QString & data);
    Q_INVOKABLE QString getData();

signals:
    void dataChanged(const QString & data);

private:
    QString myData{ "default" };
};
