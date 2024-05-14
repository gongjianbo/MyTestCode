SOURCES += \
    $$PWD/MyProto.grpc.pb.cc \
    $$PWD/MyProto.pb.cc

HEADERS += \
    $$PWD/MyProto.grpc.pb.h \
    $$PWD/MyProto.pb.h

INCLUDEPATH += $$PWD/../3rd/include
DEPENDPATH += $$PWD/../3rd/include
LIBS += $$PWD/../3rd/lib/*.lib
LIBS += -lWs2_32
