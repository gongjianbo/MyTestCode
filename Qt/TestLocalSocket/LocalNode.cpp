#include "LocalNode.h"
#include <QDebug>

LocalNodeCore::LocalNodeCore(QObject *parent)
    : QObject(parent)
{
}

LocalNodeCore::~LocalNodeCore()
{

}

bool LocalNodeCore::isConnected() const
{
    return (socket && socket->state() == QLocalSocket::ConnectedState);
}

void LocalNodeCore::init(const QString &name, bool autoconnect)
{
    qDebug()<<"init client core"<<name<<autoconnect;
    freeSocket();
    serverName = name;
    autoConnect = autoconnect;
    socket = new QLocalSocket(this);
    initSocket(socket);
    if(autoConnect){
        connectTimer = new QTimer(this);
        connectTimer->setInterval(5000);
        connect(connectTimer, &QTimer::timeout, this, [this]{
            if(autoConnect && connectTimer && !isConnected())
                socket->connectToServer(serverName, QLocalSocket::ReadWrite);
        });
        connectTimer->start();
    }
    socket->connectToServer(this->serverName, QLocalSocket::ReadWrite);
}

void LocalNodeCore::init(QLocalSocket *localsocket)
{
    qDebug()<<"init client core by socket";
    freeSocket();
    socket = localsocket;
    socket->setParent(this);
    serverName.clear();
    autoConnect = false;
    //socket parent暂时不用管，server自己处理
    initSocket(socket);
}

void LocalNodeCore::initSocket(QLocalSocket *socket)
{
    connect(socket, &QLocalSocket::readyRead, this, &LocalNodeCore::readData);
    connect(socket, &QLocalSocket::connected, this, [this]{
        if(autoConnect && connectTimer)
            connectTimer->stop();
        qDebug()<<"socket connected";
        emit netConnected();
    });
    connect(socket, &QLocalSocket::disconnected, this, [this]{
        if(autoConnect && connectTimer)
            connectTimer->start();
        qDebug()<<"socket disconnected";
        recvTemp.clear();
        emit netDisconnected();
    });
}

void LocalNodeCore::abortSocket()
{
    if(socket){
        autoConnect = false;
        if(connectTimer)
            connectTimer->stop();
        socket->abort();
        socket->disconnect();
        recvTemp.clear();
    }
}

void LocalNodeCore::freeSocket()
{
    if(socket){
        abortSocket();
        socket->deleteLater();
        socket = nullptr;
    }
}

void LocalNodeCore::readData()
{
    if(socket && socket->bytesAvailable() > 0){
        //如果有大量数据，这里要修改
        recvTemp += socket->readAll();
    }
    //数据头[4]+不含头的长度[4]+一级类型[1]+二级类型[1]+数据[N]+数据尾/校验[2]
    //4+4+1+1+2=12
    while(recvTemp.count() >= 12){
        //校验帧头
        if(memcmp(recvTemp.constData(), Local_Msg_Head, 4) == 0){
            //取帧长进行校验
            int frame_length=0;
            memcpy(&frame_length, recvTemp.constData()+4, 4);
            if(frame_length >= 8){
                //判断该帧数据是否接收完
                if(frame_length <= recvTemp.count()+4){
                    //校验帧尾
                    if(memcmp(recvTemp.constData()+2+frame_length, Local_Msg_Tail, 2) == 0){
                        //取数据，frame_length-8为去掉了头4尾2和2类型
                        filterRecv(recvTemp.at(8), recvTemp.at(9), recvTemp.mid(10, frame_length-8));
                        //从缓存移除这一帧，继续判断下一帧
                        recvTemp.remove(0, 4+frame_length);
                    }else{
                        //帧尾校验失败，后移一个帧尾的长度
                        recvTemp.remove(0, 4);
                    }
                }else{
                    //该帧数据未接受完，退出循环
                    break;
                }
            }else{
                //帧长校验失败，后移一个帧头的长度
                recvTemp.remove(0, 4);
            }
        }else{
            //帧头校验失败，后移一位继续校验
            recvTemp.remove(0, 1);
        }
    }
}

void LocalNodeCore::filterRecv(quint8 major, quint8 minor, const QByteArray &data)
{
    qDebug()<<"socket recv data"<<major<<minor;
    //目前没有过滤项
    emit recvData(major, minor, data);
}

void LocalNodeCore::sendData(quint8 major, quint8 minor, const QByteArray &data)
{
    if(!isConnected()){
        qDebug()<<"socket send data error";
        return;
    }
    qDebug()<<"socket send data"<<major<<minor;

    //是分段发送还是将头尾和内容拼接在一起后发送？
    const int frame_length = 8+data.count();
    QByteArray send_temp;
    send_temp.resize(12+data.count());
    memcpy(send_temp.data()+0, Local_Msg_Head, 4);
    memcpy(send_temp.data()+4, &frame_length, 4);
    memcpy(send_temp.data()+8, &major, 1);
    memcpy(send_temp.data()+9, &minor, 1);
    memcpy(send_temp.data()+10, data.constData(), data.count());
    memcpy(send_temp.data()+10+data.count(), Local_Msg_Tail, 2);

    socket->write(send_temp);
    socket->waitForBytesWritten();
}


void LocalNode::nodeInit(const QString &name, bool autoConnect)
{
    core = new LocalNodeCore(this);
    connect(core, &LocalNodeCore::netConnected, this, &LocalNode::netConnected);
    connect(core, &LocalNodeCore::netDisconnected, this, &LocalNode::netDisconnected);
    connect(core, &LocalNodeCore::recvData, this, &LocalNode::recvData);

    //进程定时器延时启动
    core->init(name, autoConnect);

    //发送失败后重发
    sendTimer = new QTimer(this);
    connect(sendTimer, &QTimer::timeout, this, &LocalNode::resendTemp);
}

void LocalNode::nodeInit(LocalNodeCore *newcore)
{
    core = newcore;
    core->setParent(this);
    connect(core, &LocalNodeCore::netConnected, this, &LocalNode::netConnected);
    connect(core, &LocalNodeCore::netDisconnected, this, &LocalNode::netDisconnected);
    connect(core, &LocalNodeCore::recvData, this, &LocalNode::recvData);
}

void LocalNode::nodeAbort()
{
    if(core)
        core->abortSocket();
    clearTemp();
}

void LocalNode::nodeFree()
{
    if(core)
        core->freeSocket();
    clearTemp();
}

void LocalNode::clearTemp()
{
    waitSendTemp.clear();
}

void LocalNode::netConnected()
{
    if(!waitSendTemp.isEmpty() && sendTimer)
        sendTimer->start(2000);
}

void LocalNode::netDisconnected()
{
    if(sendTimer)
        sendTimer->stop();
}

void LocalNode::recvData(quint8 major, quint8 minor, const QByteArray &data)
{
    Q_UNUSED(major)
    Q_UNUSED(minor)
    Q_UNUSED(data)
}

bool LocalNode::sendData(quint8 major, quint8 minor, const QByteArray &data, bool resend)
{
    //先发送完重发的数据，保证顺序
    resendTemp();
    //已连接就发送任务信息
    if(core && core->isConnected()){
        core->sendData(major, minor, data);
        return true;
    }

    //暂时不能发，先放到待发送列表
    if(resend){
        waitSendTemp.append(LocalTask{major, minor, data});
    }
    return false;
}

void LocalNode::resendTemp()
{
    if(core && core->isConnected()){
        while(!waitSendTemp.isEmpty())
        {
            const LocalTask &task = waitSendTemp.first();
            //可能在发的过程中断开
            if(core && core->isConnected()){
                core->sendData(task.major, task.minor, task.data);
                waitSendTemp.removeFirst();
            }else{
                break;
            }
        }
    }
    if(waitSendTemp.isEmpty() && sendTimer){
        sendTimer->stop();
    }
}
