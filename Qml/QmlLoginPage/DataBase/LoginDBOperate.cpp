#include "LoginDBOperate.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>

LoginDBOperate::LoginDBOperate(QObject *parent)
    : QObject(parent)
{
    init();
}

LoginDBOperate::~LoginDBOperate()
{

}

QStringList LoginDBOperate::getAccounts()
{
    QStringList user_list;
    if(!getOpened())
        return user_list;
    QSqlQuery query(db);
    //根据操作日期查询最新的五条数据
    //两个参数得limit 0,1，第一个为起始，第二个为偏移
    const QString sql=QString("select account from user_table order by logintime desc limit 5;");

    if(query.exec(sql)){
        while(query.next()){
            //QSqlRecord record=query.record();
            user_list<<query.value(0).toString();
        }
    }else{
        qDebug()<<"db search user list failed"<<db.lastError();
    }
    return user_list;
}

bool LoginDBOperate::getOpened() const
{
    return db.isOpen();
}

void LoginDBOperate::setOpened(bool open)
{
    if(open){
        if(db.open()){
            qDebug()<<"db open successed";
        }else{
            qDebug()<<"db open failed";
            if(db.isOpenError()){
                qDebug()<<db.lastError();
            }
        }
    }else{
        db.close();
    }
    emit openedChanged();
}

QJsonObject LoginDBOperate::lastUserInfo()
{
    if(getOpened()){
        QSqlQuery query(db);
        const QString sql=QString("select account,savepassword,password from user_table order by logintime desc limit 1;");
        if(query.exec(sql)){
            if(query.next()){
                QJsonObject user_info;
                user_info.insert("account",query.value(0).toString());
                user_info.insert("savepassword",query.value(1).toBool());
                user_info.insert("password",query.value(2).toString());
                return user_info;
            }
        }else{
            qDebug()<<"db check user info failed"<<db.lastError();
        }
    }
    QJsonObject user_info;
    user_info.insert("account","");
    user_info.insert("savepassword",false);
    user_info.insert("password","");
    return user_info;
}

QJsonObject LoginDBOperate::searchUserInfo(const QString &account)
{
    if(getOpened()&&!account.isEmpty()){
        QSqlQuery query(db);
        const QString sql=QString("select savepassword,password from user_table where account='%1';").arg(account);
        if(query.exec(sql)){
            if(query.next()){
                QJsonObject user_info;
                user_info.insert("account",account);
                user_info.insert("savepassword",query.value(0).toBool());
                user_info.insert("password",query.value(1).toString());
                return user_info;
            }
        }else{
            qDebug()<<"db check user info failed"<<db.lastError();
        }
    }
    QJsonObject user_info;
    user_info.insert("account","");
    user_info.insert("savepassword",false);
    user_info.insert("password","");
    return user_info;
}

void LoginDBOperate::saveUserInfo(const QString &account, bool savepassword, const QString &password)
{
    if(!getOpened()||account.isEmpty()||password.isEmpty())
        return;
    if(checkUserExists(account)){
        updateUserInfo(account,savepassword,password);
    }else{
        insertUserInfo(account,savepassword,password);
    }
    emit accountsChanged();
}

void LoginDBOperate::forgetUserInfo(const QString &account)
{
    if(!getOpened()||account.isEmpty())
        return;
    if(checkUserExists(account)){
        updateUserInfo(account,false,"none");
    }
    emit accountsChanged();
}

void LoginDBOperate::init()
{
    connectDB("LoginDBOperate","users.db");
}

void LoginDBOperate::connectDB(const QString &connectName, const QString &dbPath)
{
    if(QSqlDatabase::contains(connectName)){
        db=QSqlDatabase::database(connectName);
    }else{
        db=QSqlDatabase::addDatabase("QSQLITE",connectName);
    }
    db.setDatabaseName(dbPath);
    if(db.open()){
        initTable();
        db.close();
    }
}

void LoginDBOperate::initTable()
{
    QSqlQuery query(db);
    //"id INTEGER PRIMARY KEY ASC AUTOINCREMENT NOT NULL,"
    const QString sql=QString("create table if not exists user_table("
                              "account      VARCHAR (50) UNIQUE NOT NULL,"
                              "savepassword BOOLEAN NOT NULL,"
                              "password     VARCHAR (50),"
                              "logintime    TIME NOT NULL DEFAULT ( datetime('now', 'localtime') )"
                              ");");
    if(query.exec(sql)){
        qDebug()<<"init db successed";
    }else{
        qDebug()<<"init db failed"<<db.lastError();
    }
}

bool LoginDBOperate::checkUserExists(const QString &account) const
{
    QSqlQuery query(db);
    const QString sql=QString("select count(*) from user_table where account='%1';").arg(account);
    if(query.exec(sql)){
        if(query.next()){
            const int counts=query.value(0).toInt();
            if(counts>0)
                return true;
        }
    }else{
        qDebug()<<"db check user info failed"<<db.lastError();
    }
    return false;
}

void LoginDBOperate::insertUserInfo(const QString &account, bool savepassword, const QString &password)
{
    QSqlQuery query(db);
    const QString sql=QString("insert into user_table(account,savepassword,password)"
                              "values('%1',%2,'%3');").arg(account).arg(savepassword).arg(password);
    if(query.exec(sql)){
    }else{
        qDebug()<<"db insert user info failed"<<db.lastError();
    }
}

void LoginDBOperate::updateUserInfo(const QString &account, bool savepassword, const QString &password)
{
    QSqlQuery query(db);
    const QString sql=QString("update user_table set savepassword=%2,"
                              "password='%3',logintime=datetime('now', 'localtime') "
                              "where account='%1';").arg(account).arg(savepassword).arg(password);
    if(query.exec(sql)){
    }else{
        qDebug()<<"db insert user info failed"<<db.lastError();
    }
}
