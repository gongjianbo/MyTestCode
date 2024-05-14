#include "MyService.h"
#include <QFile>
#include <QThread>
#include <QDateTime>
#include <QDebug>

void MyService::initService()
{
    std::lock_guard<std::mutex> lock(connectionMtx); (void)lock;
    connections.clear();
}

grpc::Status MyService::login(grpc::ServerContext *context,
                              const MyPackage::UserInfo *request,
                              MyPackage::Empty *response)
{
    {
        std::lock_guard<std::mutex> lock(connectionMtx); (void)lock;
        (void)response;
        auto &&client = context->peer();
        MyConnection con;
        con.client = client;
        con.user = request->user();
        con.syncTime = QDateTime::currentDateTime().toSecsSinceEpoch();
        // 如果 client 存在就覆盖
        connections[client] = con;
    }
    //
    QString log = QString("[%1][%2] Login")
                      .arg(QString::fromStdString(context->peer()))
                      .arg(quintptr(QThread::currentThreadId()));
    emit updateLog(log);
    return grpc::Status::OK;
}

grpc::Status MyService::logout(grpc::ServerContext *context,
                               const MyPackage::UserInfo *request,
                               MyPackage::Empty *response)
{
    {
        std::lock_guard<std::mutex> lock(connectionMtx); (void)lock;
        (void)response;
        (void)request;
        auto &&client = context->peer();
        auto &&it = connections.find(client);
        if (it != connections.end()) {
            connections.erase(it);
        }
    }
    //
    QString log = QString("[%1][%2] Logout")
                      .arg(QString::fromStdString(context->peer()))
                      .arg(quintptr(QThread::currentThreadId()));
    emit updateLog(log);
    return grpc::Status::OK;
}

grpc::Status MyService::syncMessage(grpc::ServerContext *context,
                                    const MyPackage::MsgInfo *request,
                                    grpc::ServerWriter<MyPackage::MsgInfo> *writer)
{
    std::lock_guard<std::mutex> lock(connectionMtx); (void)lock;
    (void)request;
    auto &&client = context->peer();
    auto &&it = connections.find(client);
    if (it != connections.end()) {
        auto &msgs = it->second.msgs;
        for (auto &&msg : msgs)
        {
            MyPackage::MsgInfo response;
            response.set_type(msg.type);
            response.set_msg(msg.msg);
            writer->Write(response);
        }
        msgs.clear();
        it->second.syncTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    }
    return grpc::Status::OK;
}

grpc::Status MyService::insertImage(grpc::ServerContext *context,
                                    const MyPackage::ImageInfo *request,
                                    MyPackage::ImageInfo *response)
{
    // 为了防止阻塞其他客户端调用，不要把一些不必要的操作锁进去
    QThread::msleep(20 * 1000);
    std::lock_guard<std::mutex> lock(dataMtx); (void)lock;
    //
    QFile file("cache.png");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QByteArray::fromStdString(request->data()));
        file.close();
    } else {
        return grpc::Status::CANCELLED;
    }
    QString log = QString("[%1][%2] Insert Image")
                      .arg(QString::fromStdString(context->peer()))
                      .arg(quintptr(QThread::currentThreadId()));
    emit updateLog(log);
    //
    cacheMessage(context->peer(), MyPackage::InsertImageMsg, log.toStdString());
    return grpc::Status::OK;
}

grpc::Status MyService::deleteImage(grpc::ServerContext *context,
                                    const MyPackage::ImageInfo *request,
                                    MyPackage::ImageInfo *response)
{
    std::lock_guard<std::mutex> lock(dataMtx); (void)lock;
    //
    QString log = QString("[%1][%2] Delete Image")
                      .arg(QString::fromStdString(context->peer()))
                      .arg(quintptr(QThread::currentThreadId()));
    emit updateLog(log);
    //
    cacheMessage(context->peer(), MyPackage::DeleteImageMsg, log.toStdString());
    return grpc::Status::OK;
}

grpc::Status MyService::updateImage(grpc::ServerContext *context,
                                    const MyPackage::ImageInfo *request,
                                    MyPackage::ImageInfo *response)
{
    std::lock_guard<std::mutex> lock(dataMtx); (void)lock;
    //
    QString log = QString("[%1][%2] Update Image")
                      .arg(QString::fromStdString(context->peer()))
                      .arg(quintptr(QThread::currentThreadId()));
    emit updateLog(log);
    //
    cacheMessage(context->peer(), MyPackage::UpdateImageMsg, log.toStdString());
    return grpc::Status::OK;
}

grpc::Status MyService::selectImage(grpc::ServerContext *context,
                                    const MyPackage::ImageInfo *request,
                                    MyPackage::ImageInfo *response)
{
    std::lock_guard<std::mutex> lock(dataMtx); (void)lock;
    //
    QString log = QString("[%1][%2] Select Image")
                      .arg(QString::fromStdString(context->peer()))
                      .arg(quintptr(QThread::currentThreadId()));
    emit updateLog(log);
    // 查询不需要通知
    // cacheMessage(context->peer(), MyPackage::SelectImageMsg, log.toStdString());
    return grpc::Status::OK;
}

void MyService::cacheMessage(const std::string &client, MyPackage::MsgType type, const std::string &message)
{
    std::lock_guard<std::mutex> lock(connectionMtx); (void)lock;
    auto &&cur_time =  QDateTime::currentDateTime().toSecsSinceEpoch();
    qDebug()<<"cache message"<<"connections"<<connections.size()<<QThread::currentThread();
    for (auto it = connections.begin(); it != connections.end();)
    {
        if (it->second.client == client) {
            ++it;
            continue;
        }
        if (it->second.syncTime + 5 * 60 < cur_time) {
            connections.erase(it++);
        } else {
            MyMessage msg{type, message};
            it->second.msgs.push_back(msg);
            ++it;
        }
    }
}
