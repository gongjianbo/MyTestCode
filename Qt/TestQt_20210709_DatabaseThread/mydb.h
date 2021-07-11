#ifndef MYDB_H
#define MYDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QDebug>
#include <mutex>

class MyDb : public QObject
{
    Q_OBJECT
private:
    explicit MyDb(QObject *parent = nullptr);
public:
    static MyDb *getInstance();

    //随便加两个操作接口用于测试
    void insertData();
    void selectData();

private:
    void initDb();

private:
    QSqlDatabase db;
    std::mutex dbMutex;
};

#endif // MYDB_H
