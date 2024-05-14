#pragma once
#include <QObject>
#include <memory>
#include <thread>
#include <map>
#include <mutex>
#include <atomic>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "MyProto.grpc.pb.h"

// 实现服务端接口
class MyService : public QObject, public MyPackage::MyServer::Service
{
    Q_OBJECT
public:
    using QObject::QObject;

    // 初始化
    void initService();

    // 客户端登录，登陆后添加到已连接列表，开始缓存待推送消息
    grpc::Status login(grpc::ServerContext *context,
                       const MyPackage::UserInfo *request,
                       MyPackage::Empty *response) override;
    // 客户端退出，从列表移除
    grpc::Status logout(grpc::ServerContext *context,
                        const MyPackage::UserInfo *request,
                        MyPackage::Empty *response) override;
    // 消息查询，将缓存的消息全部发送给客户端
    grpc::Status syncMessage(grpc::ServerContext *context,
                             const MyPackage::MsgInfo *request,
                             grpc::ServerWriter<MyPackage::MsgInfo> *writer) override;

    // 新增图片
    grpc::Status insertImage(grpc::ServerContext *context,
                             const MyPackage::ImageInfo *request,
                             MyPackage::ImageInfo *response) override;
    // 删除图片
    grpc::Status deleteImage(grpc::ServerContext *context,
                             const MyPackage::ImageInfo *request,
                             MyPackage::ImageInfo *response) override;
    // 修改图片
    grpc::Status updateImage(grpc::ServerContext *context,
                             const MyPackage::ImageInfo *request,
                             MyPackage::ImageInfo *response) override;
    // 查询图片
    grpc::Status selectImage(grpc::ServerContext *context,
                             const MyPackage::ImageInfo *request,
                             MyPackage::ImageInfo *response) override;

signals:
    void updateLog(const QString &log);

private:
    void cacheMessage(const std::string &client, MyPackage::MsgType type, const std::string &message);

private:
    // 加锁
    std::mutex dataMtx;
    std::mutex connectionMtx;
    // 消息内容
    struct MyMessage {
        // 消息类型
        MyPackage::MsgType type;
        // 消息内容
        std::string msg;
    };
    // 客户端结构
    struct MyConnection {
        // 客户端地址
        std::string client;
        // 用户信息
        std::string user;
        // 最近同步时间
        int64_t syncTime;
        // 待同步的消息队列
        std::vector<MyMessage> msgs;
    };
    // 当前已连接的客户端
    std::map<std::string, MyConnection> connections;
};
