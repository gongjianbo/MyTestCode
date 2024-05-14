#include "MyNode.h"
#include <QThread>
#include <QRandomGenerator>
#include <QFile>
#include <QDebug>

bool MyNode::login(const QString &host)
{
    std::lock_guard<std::recursive_mutex> lock(mtx); (void)lock;
    if (channel) {
        doLogout();
    }
    grpc::ChannelArguments args;
    // 消息默认限制 4M
    const int max_size = 1024 * 1024 * 1024;
    args.SetMaxReceiveMessageSize(max_size);
    args.SetMaxSendMessageSize(max_size);
    channel = grpc::CreateCustomChannel(host.toStdString(), grpc::InsecureChannelCredentials(), args);
    if (channel && !stub) {
        stub = MyPackage::MyServer::NewStub(channel);
    }
    if (channel && stub) {
        grpc::ClientContext context;
        MyPackage::UserInfo request;
        request.set_user("xxx");
        MyPackage::Empty response;
        grpc::Status status = stub->login(&context, request, &response);
        if (status.ok()) {
            liveCounter = 0;
            running = true;
            auto th = new std::thread([this](){
                bool ret = true;
                qDebug()<<"sync start";
                while (ret && running) {
                    QThread::msleep(QRandomGenerator::global()->bounded(800, 1200));
                    if (mtx.try_lock()) {
                        ret = doSyncMessage();
                        mtx.unlock();
                    }
                }
                // 退出或者重连
                if (running) {
                    emit updateLog("Error");
                }
                qDebug()<<"sync quit";
            });
            syncThread.reset(th);
            emit updateLog("Login");
            return true;
        }
    }
    doLogout();
    return false;
}

bool MyNode::logout()
{
    std::lock_guard<std::recursive_mutex> lock(mtx); (void)lock;
    if (!channel)
        return true;
    if (stub) {
        grpc::ClientContext context;
        MyPackage::UserInfo request;
        request.set_user("xxx");
        MyPackage::Empty response;
        grpc::Status status = stub->logout(&context, request, &response);
    }
    doLogout();
    return true;
}

bool MyNode::insertImage(const QString &filepath)
{
    std::lock_guard<std::recursive_mutex> lock(mtx); (void)lock;
    if (!stub || filepath.isEmpty())
        return false;
    grpc::ClientContext context;
    MyPackage::ImageInfo request;
    request.set_comment("comment");
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    request.set_data(file.readAll().toStdString());
    MyPackage::ImageInfo response;
    grpc::Status status = stub->insertImage(&context, request, &response);
    return status.ok();
}

bool MyNode::deleteImage()
{
    std::lock_guard<std::recursive_mutex> lock(mtx); (void)lock;
    if (!stub)
        return false;
    grpc::ClientContext context;
    MyPackage::ImageInfo request;
    MyPackage::ImageInfo response;
    grpc::Status status = stub->deleteImage(&context, request, &response);
    return status.ok();
}

bool MyNode::updateImage()
{
    std::lock_guard<std::recursive_mutex> lock(mtx); (void)lock;
    if (!stub)
        return false;
    grpc::ClientContext context;
    MyPackage::ImageInfo request;
    MyPackage::ImageInfo response;
    grpc::Status status = stub->updateImage(&context, request, &response);
    return status.ok();
}

bool MyNode::selectImage()
{
    std::lock_guard<std::recursive_mutex> lock(mtx); (void)lock;
    if (!stub)
        return false;
    grpc::ClientContext context;
    MyPackage::ImageInfo request;
    MyPackage::ImageInfo response;
    grpc::Status status = stub->selectImage(&context, request, &response);
    return status.ok();
}

void MyNode::doLogout()
{
    running = false;
    if (syncThread && syncThread->joinable()) {
        syncThread->join();
        syncThread.reset();
    }
    if (stub) {
        stub.reset();
    }
    if (channel) {
        channel.reset();
    }
    emit updateLog("Logout");
}

bool MyNode::doSyncMessage()
{
    if (!stub)
        return false;
    grpc::ClientContext context;
    MyPackage::MsgInfo request;
    MyPackage::MsgInfo response;
    auto &&stream = stub->syncMessage(&context, request);
    if (!stream) {
        liveCounter++;
        return liveCounter < 3;
    }
    while (running && stream && stream->Read(&response)) {
        qDebug()<<"read message"
                 <<"type"<<response.type()
                 <<"msg"<<QString::fromStdString(response.msg());
        //
        QString log = QString::fromStdString(response.msg());
        emit updateLog(log);
    }
    // GetState 需要请求之后再调用，否则是默认状态
    grpc_connectivity_state cs = channel->GetState(true);
    // qDebug()<<"read state"<<cs;
    if (cs != GRPC_CHANNEL_READY) {
        liveCounter++;
        return liveCounter < 3;
    } else {
        liveCounter = 0;
    }
    return true;
}
