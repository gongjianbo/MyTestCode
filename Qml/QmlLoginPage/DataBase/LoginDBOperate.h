#ifndef LOGINDBOPERATE_H
#define LOGINDBOPERATE_H

#include <QObject>
#include <QSqlDatabase>

#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

class LoginDBOperate : public QObject
{
    Q_OBJECT
    //最近操作的账号列表
    Q_PROPERTY(QStringList accounts READ getAccounts NOTIFY accountsChanged)
    //数据库open状态
    Q_PROPERTY(bool opened READ getOpened WRITE setOpened NOTIFY openedChanged)
public:
    explicit LoginDBOperate(QObject *parent = nullptr);
    ~LoginDBOperate();

    QStringList getAccounts();

    bool getOpened() const;
    void setOpened(bool open);

    //最后登陆的用户登陆信息
    Q_INVOKABLE QJsonObject lastUserInfo();
    //根据用户名查询用户登陆信息
    Q_INVOKABLE QJsonObject searchUserInfo(const QString &account);
    //保存用户登陆信息
    Q_INVOKABLE void saveUserInfo(const QString &account,bool savepassword,const QString &password);
    //抹掉用户的密码信息
    Q_INVOKABLE void forgetUserInfo(const QString &account);

private:
    void init();
    void connectDB(const QString &connectName, const QString &dbPath);
    void initTable();
    bool checkUserExists(const QString &account) const;
    void insertUserInfo(const QString &account,bool savepassword,const QString &password);
    void updateUserInfo(const QString &account,bool savepassword,const QString &password);

signals:
    void accountsChanged();
    void openedChanged();

private:
    QSqlDatabase db;
};

#endif // LOGINDBOPERATE_H
