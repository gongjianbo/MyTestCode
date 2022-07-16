#include "MyLine.h"
#include <QtQuick/QSGNode>
#include "MyGeometry.h"
#include "MyMaterial.h"

MyLine::MyLine(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QSGNode *MyLine::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    QSGGeometryNode *node = nullptr;
    MyGeometry *geometry = nullptr;

    if (!oldNode) {
        node = new QSGGeometryNode;

        geometry = new MyGeometry(MyGeometry::defaultAttributes_My2D(), 2);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        MyMaterial *material = new MyMaterial;
        //material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = static_cast<MyGeometry *>(node->geometry());
    }

    MyGeometry::My2D *vertices = geometry->vertexDataAsMy2D();
    vertices[0].set(0,0,255,0,0,50); //半透明红色
    vertices[1].set(width(),height(),0,0,255); //蓝色
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
