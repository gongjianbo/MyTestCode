#include "LocalHost.h"
#include <QDebug>

void LocalHost::hostInit(const QString &name)
{
    qDebug()<<"init host"<<name;
    serverName = name;
    QLocalServer::removeServer(serverName);
    server = new QLocalServer(this);
    if(server->listen(serverName)){
        qDebug()<<"host listen successful";
        connect(server, &QLocalServer::newConnection, this, &LocalHost::newConnection);
    }else{
        qDebug()<<"host listen failed";
    }
}

void LocalHost::hostFree()
{
    qDebug()<<"free host"<<serverName;
    server->close();
}

void LocalHost::newConnection()
{
    if(!server->hasPendingConnections())
        return;
    QLocalSocket *socket = server->nextPendingConnection();
    if(!socket)
        return;
    qDebug()<<"host new node connection";
    LocalNodeCore *core = new LocalNodeCore(this);
    core->init(socket);
    emit newNode(core);
}
