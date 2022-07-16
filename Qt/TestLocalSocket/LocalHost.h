#pragma once
#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QTimer>
#include "LocalDefine.h"
#include "LocalNode.h"

//服务端网络接口
class LocalHost : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

signals:
    //外部接收后，变更其parent
    void newNode(LocalNodeCore *core);

public slots:
    //初始化
    void hostInit(const QString &serverName);
    void hostFree();
    //新的连接
    void newConnection();

private:
    //localsocket
    QString serverName;
    QLocalServer *server{ nullptr };
};

