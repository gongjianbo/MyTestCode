#include "MyGeometry.h"

//QSGGeometry的构造函数需要AttributeSet参数
//struct AttributeSet {
//    int count;
//    int stride;
//    const Attribute *attributes;
//};
const QSGGeometry::AttributeSet &MyGeometry::defaultAttributes_My2D()
{
    static Attribute data[] = {
        //对应material的坐标vertex
        Attribute::createWithAttributeType(0, 2, FloatType, PositionAttribute),
        //对应material的颜色vertex
        Attribute::createWithAttributeType(1, 4, UnsignedByteType, ColorAttribute)
    };
    //count为属性个数，stride为vertex大小/步进
    //vertexByteSize = m_attributes.stride * m_vertex_count;
    static AttributeSet attrs = { 2, 2 * sizeof(float) + 4 * sizeof(char), data };
    return attrs;
}

MyGeometry::My2D *MyGeometry::vertexDataAsMy2D()
{
    return static_cast<My2D *>(vertexData());
}

const MyGeometry::My2D *MyGeometry::vertexDataAsMy2D() const
{
    return static_cast<const My2D *>(vertexData());
}
