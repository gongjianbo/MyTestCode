#include "TestServer.h"

#include <string>
#include <unordered_map>
#include <QThread>
#include <QDebug>

grpc::Status TestServer::MyCallA(grpc::ServerContext *context,
                                 const MyPackage::MyRequest *request,
                                 MyPackage::MyResponse *response)
{
    (void*)context;
    qDebug()<<"server call a id"<<(request->id())<<QThread::currentThread();
    response->set_id(request->id()+1);
    //数组repeated
    for(int i=0;i<request->arr_size();i++)
    {
        //response->set_arr(i,request->arr(i));
        response->add_arr(request->arr(i));
    }
    //结构体message
    MyPackage::MyData recv_data=request->data();
    MyPackage::MyData *send_data=response->mutable_data();
    send_data->set_id(recv_data.id()+1);
    send_data->set_name(recv_data.name()+":"+std::to_string(recv_data.name().size())+"bytes");
    send_data->set_data(recv_data.data()+":"+std::to_string(recv_data.data().size())+"bytes");
    //response->set_data(send_data);
    response->set_type(request->type());
    //字典map
    std::unordered_map<int,std::string> standard_map(request->dict().begin(),request->dict().end());
    //google::protobuf::Map<int,std::string> grpc_map(standard_map.begin(), standard_map.end());
    google::protobuf::Map<int,std::string> *grpc_map=response->mutable_dict();
    for(auto& v : standard_map){
        //(*grpc_map)[v.first]=v.second;
        grpc_map->insert({v.first,v.second});
    }
    //联合体oneof

    return grpc::Status::OK;
}

grpc::Status TestServer::MyCallB(grpc::ServerContext *context,
                                 const MyPackage::MyData *request,
                                 ::grpc::ServerWriter<MyPackage::MyData> *writer)
{
    (void*)context;
    qDebug()<<"server call a id"<<(request->id())<<QThread::currentThread();
    for(int i=0;i<5;i++)
    {
        qDebug()<<"server for"<<i;
        QThread::msleep(1000);
        MyPackage::MyData data;
        data.set_id(i);
        data.set_name("name");
        data.set_data("data");
        writer->Write(data);
    }
    return grpc::Status::OK;
}
