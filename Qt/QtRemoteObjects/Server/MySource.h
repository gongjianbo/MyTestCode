#pragma once
#include "rep_simple_source.h"

class MySource : public InterfaceSimpleSource
{
    Q_OBJECT
public:
    using InterfaceSimpleSource::InterfaceSimpleSource;

    void setData(const QString & data) override;
    QString getData() override;
    void testEnum(InterfaceSource::MyEnum t) override;
    void testStruct(MyStruct t) override;

private:
    QString myData{ "default" };
};
