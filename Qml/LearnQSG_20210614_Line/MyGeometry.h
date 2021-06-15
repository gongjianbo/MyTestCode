#pragma once
#include <QSGGeometry>

//参照源码：
//qt-everywhere-src\qtdeclarative\src\quick\scenegraph\coreapi\qsggeometry.h
//这里仿写一个ColoredPoint2D
class MyGeometry : public QSGGeometry
{
public:
    using QSGGeometry::QSGGeometry;
    //定义数据结构只是为了操作方便
    struct My2D {
        float x, y;
        unsigned char r, g, b, a;
        void set(float nx, float ny, uchar nr, uchar ng, uchar nb, uchar na = 255){
            x = nx; y = ny; r = nr; g = ng; b = nb; a = na;
        }
    };
    static const AttributeSet &defaultAttributes_My2D();

    My2D *vertexDataAsMy2D();
    const My2D *vertexDataAsMy2D() const;
};

