#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <grpcpp/grpcpp.h>
#include "MyProto.grpc.pb.h"

class TestServer : public MyPackage::MyServer::Service
{
public:
    grpc::Status MyCallA(grpc::ServerContext *context,
                         const MyPackage::MyRequest *request,
                         MyPackage::MyResponse *response) override;
    grpc::Status MyCallB(grpc::ServerContext *context,
                         const MyPackage::MyData *request,
                         ::grpc::ServerWriter<MyPackage::MyData> *writer) override;
};

#endif // TESTSERVER_H
