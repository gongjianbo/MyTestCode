#pragma once
#include <QObject>
#include <QLocalSocket>
#include <QTimer>
#include "LocalDefine.h"

//客户端网络相关接口
class LocalNodeCore : public QObject
{
    Q_OBJECT
public:
    explicit LocalNodeCore(QObject *parent=nullptr);
    ~LocalNodeCore();
    //查询网络连接状态
    bool isConnected() const;

signals:
    //连接状态
    void netConnected();
    void netDisconnected();
    //接收并校验过的数据
    void recvData(quint8 major, quint8 minor, const QByteArray &data);

public slots:
    //初始化-主动创建
    //autoConnect=true则断开连接后会自动定时重连
    void init(const QString &serverName, bool autoConnect = true);
    //初始化-以已有的socket创建
    void init(QLocalSocket *socket);
    //关联信号槽
    void initSocket(QLocalSocket *socket);
    //socket断开连接
    void abortSocket();
    //socket断开连接并释放
    void freeSocket();
    //处理收到的数据
    void readData();
    //处理解析过滤后的数据，然后发送recvData信号
    void filterRecv(quint8 major, quint8 minor, const QByteArray &data);
    //发送数据
    void sendData(quint8 major, quint8 minor, const QByteArray &data);

private:
    //local sokcet的服务名
    QString serverName;
    //网络
    QLocalSocket *socket{ nullptr };
    //自动重连
    bool autoConnect{ false };
    QTimer *connectTimer{ nullptr };

    //缓存收到的数据
    QByteArray recvTemp;
};


//客户端的接口抽象父类
class LocalNode : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    //初始化
    //autoConnect=true则断开连接后会自动定时重连
    void nodeInit(const QString &serverName, bool autoConnect = true);
    //初始化-以已有的socket创建
    void nodeInit(LocalNodeCore *core);
    //清除缓存并断开连接，终止操作时调用
    void nodeAbort();
    //释放
    void nodeFree();
    //清除缓存
    void clearTemp();

protected slots:
    //socket连接成功
    virtual void netConnected();
    //socket断开连接
    virtual void netDisconnected();
    //数据接收
    virtual void recvData(quint8 major, quint8 minor, const QByteArray &data);
    //数据发送,resend=true时如果发送失败则重发
    bool sendData(quint8 major, quint8 minor, const QByteArray &data, bool resend = false);
    //重发
    void resendTemp();

private:
    //网络收发数据
    LocalNodeCore *core{ nullptr };
    //数据重发
    QTimer *sendTimer{ nullptr };
    struct LocalTask {
        quint8 major{ 0 };
        quint8 minor{ 0 };
        QByteArray data;
    };
    //待重发的列表
    QList<LocalTask> waitSendTemp;
};

