#include "ScanTcpTask.h"
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

ScanTcpTask::ScanTcpTask(quint16 port, bool findName, QObject *parent)
    : QThread(parent), scanPort(port), needFindName(findName)
{

}

ScanTcpTask::~ScanTcpTask()
{

}

void ScanTcpTask::run()
{
    const QList<QHostAddress> addr_list = QNetworkInterface::allAddresses();
    qDebug()<<__FUNCTION__<<"address size"<<addr_list.size();
    for (const QHostAddress &address : qAsConst(addr_list))
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            qDebug()<<"\t"<<address;
            quint32 ip_base = address.toIPv4Address();
            ip_base &= 0xFFFFFF00;
            // 可以忽略：0网络地址；255广播地址
            for (quint32 offset = 0; offset < 256; offset++)
            {
                QHostAddress addr(ip_base | offset);
                QString ip = addr.toString();
                QTcpSocket *socket = new QTcpSocket;
                socket->connectToHost(ip, scanPort, QAbstractSocket::ReadWrite, QAbstractSocket::IPv4Protocol);
                connect(socket, &QTcpSocket::connected, [ip, socket, this]{
                    qDebug()<<"find"<<ip<<QTime::currentTime();
                    socket->close();

                    QString host_name;
                    if (needFindName) {
                        QHostInfo info = QHostInfo::fromName(ip);
                        // 这里可能会阻塞10s
                        host_name = info.hostName();
                        qDebug()<<"hostName"<<host_name<<QTime::currentTime();
                    }
                    emit foundAddress(ip, host_name);
                });
                socketList.push_back(socket);
            }
        }
    }

    // 通过定时器在实现超时退出
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [=](){
        // thread-safe.
        quit();
    });
    timer.start(9000);
    // 事件循环等待结束
    exec();

    clear();
    emit scanFinished();
    qDebug()<<__FUNCTION__<<"finish";
}

void ScanTcpTask::clear()
{
    for (QTcpSocket *socket : qAsConst(socketList))
    {
        if (socket) {
            socket->abort();
            socket->deleteLater();
        }
    }
    socketList.clear();
}
