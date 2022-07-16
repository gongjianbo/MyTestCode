#include "TestClient.h"

#include <QDebug>

void TestClient::callA(int id)
{
    MyPackage::MyRequest request;
    request.set_id(id);
    for(int i=1;i<5;i++){
        request.add_arr(i*i);
    }
    MyPackage::MyData *send_data=request.mutable_data();
    send_data->set_id(id);
    send_data->set_name("str name");
    send_data->set_data("bytes data");

    MyPackage::MyResponse response;
    grpc::ClientContext context;

    std::shared_ptr<grpc::Channel> channel=grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials());
    std::unique_ptr<MyPackage::MyServer::Stub> stub=MyPackage::MyServer::NewStub(channel);
    grpc::Status status=stub->MyCallA(&context,request,&response);
    qDebug()<<"client call a result"<<status.ok();
    if(status.ok()){
        qDebug()<<"id"<<response.id();
        for(int i=0;i<response.arr_size();i++)
            qDebug()<<response.arr(i);
        MyPackage::MyData data=response.data();
        qDebug()<<data.id();
        qDebug()<<QString::fromStdString(data.name());
        qDebug()<<QString::fromStdString(data.data());
    }
}

void TestClient::callB(int id)
{
    MyPackage::MyData request;
    request.set_id(id);
    request.set_name("str name");
    request.set_data("bytes data");

    MyPackage::MyData response;
    grpc::ClientContext context;

    std::shared_ptr<grpc::Channel> channel=grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials());
    std::unique_ptr<MyPackage::MyServer::Stub> stub=MyPackage::MyServer::NewStub(channel);
    std::unique_ptr<grpc::ClientReader<MyPackage::MyData>> reader(stub->MyCallB(&context,request));
    qDebug()<<"client call b";
    while(reader->Read(&response)){
        qDebug()<<response.id();
    }
    grpc::Status status=reader->Finish();
    qDebug()<<"client call b result"<<status.ok();
}
