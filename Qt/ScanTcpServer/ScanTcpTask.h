#pragma once
#include <QThread>
#include <QTcpSocket>

/**
 * @brief 查找可用的服务器ip和hostName
 * @author 龚建波
 * @date 2023-03-14
 * @details
 * 遍历同网段下所有ip，通过tcp连接固定端口
 * 默认只处理ipv4
 * 默认只处理掩码255.255.255.0的情况
 */
class ScanTcpTask : public QThread
{
    Q_OBJECT
public:
    // port: 扫描的端口，服务器先监听端口使tcp可以连接
    // findName: 是否需要获取主机名，因为获取主机名很耗时，部分场景又不需要
    explicit ScanTcpTask(quint16 port, bool findName, QObject *parent = nullptr);
    ~ScanTcpTask();

protected:
    // 线程中执行任务
    void run() override;

private:
    // 连接一个地址并将socket放到容器
    void checkAddress(const QString &ip);
    // 释放所有socket
    void clear();

signals:
    // 查找结束
    void scanFinished();
    // 查找到的地址通过信号发送出去
    void foundAddress(const QString &ip, const QString &hostName);

private:
    // 扫描的端口
    quint16 scanPort;
    // 是否需要查找主机名
    bool needFindName;
    // 遍历时每个ip创建一个socket去连接，放到容器便于释放
    QList<QTcpSocket*> socketList;
};
