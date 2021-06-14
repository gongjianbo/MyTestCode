#include "SmoothLine.h"

#include <QSGGeometry>
#include <QSGGeometryNode>

struct Color4ub
{
    unsigned char r, g, b, a;
};

inline Color4ub colorToColor4ub(const QColor &c)
{
    Color4ub color = { uchar(c.redF() * c.alphaF() * 255),
                       uchar(c.greenF() * c.alphaF() * 255),
                       uchar(c.blueF() * c.alphaF() * 255),
                       uchar(c.alphaF() * 255)
                     };
    return color;
}

struct SmoothVertex
{
    float x, y;
    Color4ub color;
    float dx, dy;
    void set(float nx, float ny, Color4ub ncolor, float ndx, float ndy)
    {
        x = nx; y = ny; color = ncolor;
        dx = ndx; dy = ndy;
    }
};

const QSGGeometry::AttributeSet &smoothAttributeSet()
{
    static QSGGeometry::Attribute data[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 4, GL_UNSIGNED_BYTE, false),
        QSGGeometry::Attribute::create(2, 2, GL_FLOAT, false)
    };
    static QSGGeometry::AttributeSet attrs = { 3, sizeof(SmoothVertex), data };
    return attrs;
}

SmoothLine::SmoothLine(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QSGNode *SmoothLine::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry;
    SmoothMaterial *material;
    node = static_cast<QSGGeometryNode *>(oldNode);
    if(!node) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(smoothAttributeSet(), 0);
        geometry->setDrawingMode(GL_TRIANGLE_STRIP);
        material = new SmoothMaterial();
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        geometry = node->geometry();
        material = static_cast<SmoothMaterial *>(node->material());
    }

    int vertexStride = geometry->sizeOfVertex();
    int vertexCount = 8;

    geometry->allocate(vertexCount, 0);
    SmoothVertex *smoothVertices = reinterpret_cast<SmoothVertex *>(geometry->vertexData());
    memset(smoothVertices, 0, vertexCount * vertexStride);

    float lineWidth = 1;
    float tlX = 0;   float tlY = 0;               //top-left
    float blX = 0;   float blY = 0 + lineWidth;   //bottom-left
    float trX = width(); float trY = height();             //top-right
    float brX = width(); float brY = height() + lineWidth; //bottom-right
    float delta = lineWidth * 0.5f;

    Color4ub fillColor = colorToColor4ub(QColor(255,0,0,255));
    Color4ub transparent = { 0, 0, 0, 0 };

    smoothVertices[0].set(trX, trY, transparent, delta, -delta);
    smoothVertices[1].set(tlX, tlY, transparent, -delta, -delta);

    smoothVertices[2].set(trX, trY, fillColor, -delta, delta);
    smoothVertices[3].set(tlX, tlY, fillColor, delta, delta);
    smoothVertices[4].set(brX, brY, fillColor, -delta, -delta);
    smoothVertices[5].set(blX, blY, fillColor, delta, -delta);

    smoothVertices[6].set(brX, brY, transparent, delta, delta);
    smoothVertices[7].set(blX, blY, transparent, -delta, delta);

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
