#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include <grpcpp/grpcpp.h>
#include "MyProto.grpc.pb.h"

class TestClient
{
public:
    void callA(int id);
    void callB(int id);
};

#endif // TESTCLIENT_H
