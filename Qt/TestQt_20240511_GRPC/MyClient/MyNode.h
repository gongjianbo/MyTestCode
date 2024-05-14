#pragma once
#include <QObject>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <grpcpp/grpcpp.h>
#include "MyProto.grpc.pb.h"

class MyNode : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    // 客户端连接服务器
    bool login(const QString &host);
    // 断开连接
    bool logout();

    // 新增图片
    bool insertImage(const QString &filepath);
    // 删除图片
    bool deleteImage();
    // 修改图片
    bool updateImage();
    // 查询图片
    bool selectImage();


signals:
    void updateLog(const QString &log);

private:
    // 断开连接
    void doLogout();
    // 读取消息
    bool doSyncMessage();

private:
    // 加锁
    std::recursive_mutex mtx;
    // channel 提供了 tcp 层的连接，而 stub 则提供了应用层的连接
    // 所以单个客户端只需要一个 channel，而每个封装的类可以有自己的 stub
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<MyPackage::MyServer::Stub> stub;
    // 同步消息
    std::atomic_int64_t liveCounter{0};
    std::unique_ptr<std::thread> syncThread;
    std::atomic_bool running{false};
};
