protoc.exe --cpp_out=./ MyProto.proto
protoc.exe --grpc_out=./ --plugin=protoc-gen-grpc=grpc_cpp_plugin.exe MyProto.proto
pause