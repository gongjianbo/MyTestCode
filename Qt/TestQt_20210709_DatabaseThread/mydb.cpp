#include "mydb.h"

MyDb::MyDb(QObject *parent) : QObject(parent)
{
    initDb();
}

MyDb *MyDb::getInstance()
{
    static MyDb instance;
    return &instance;
}

void MyDb::insertData()
{
    std::lock_guard<std::mutex> guard(dbMutex); Q_UNUSED(guard)
            QSqlQuery query(db);
    const QString sql="insert into mytable(name,age) values('test',20)";
    if(!query.exec(sql)){
        qDebug()<<"insert failed."<<query.lastError()<<QThread::currentThread();
    }else{
        qDebug()<<"insert success."<<QThread::currentThread();
    }
}

void MyDb::selectData()
{
    std::lock_guard<std::mutex> guard(dbMutex); Q_UNUSED(guard)
            QSqlQuery query(db);
    const QString sql="select * from mytable";
    if(!query.exec(sql)){
        qDebug()<<"select failed."<<query.lastError()<<QThread::currentThread();
    }else{
        qDebug()<<"select success."<<QThread::currentThread();
        std::thread th([&]{
            qDebug()<<"select print."<<QThread::currentThread();
            int index=0;
            while(query.next()){
                qDebug()<<index<<query.value("name").toString()<<query.value("age").toInt();
                index++;
            }
        });
        th.join();
    }
}

void MyDb::initDb()
{
    std::lock_guard<std::mutex> guard(dbMutex); Q_UNUSED(guard)
            if(QSqlDatabase::contains("mydb_connection")){
        db=QSqlDatabase::database("mydb_connection");
    }else{
        db=QSqlDatabase::addDatabase("QSQLITE","mydb_connection");
    }
    db.setDatabaseName("mydb.db");
    if(!db.open()){
        qDebug()<<"open db error."<<db.lastError()<<QThread::currentThread();
        return;
    }else{
        qDebug()<<"open db success."<<QThread::currentThread();
    }

    //创建表格
    QSqlQuery query(db);
    const QString sql="CREATE TABLE IF NOT EXISTS mytable("
"id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
"name TEXT, "
"age INTEGER)";
    if(!query.exec(sql)){
        qDebug()<<"init table failed."<<query.lastError()<<QThread::currentThread();
    }else{
        qDebug()<<"init table success."<<QThread::currentThread();
    }
}
