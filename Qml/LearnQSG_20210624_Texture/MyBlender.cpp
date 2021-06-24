#include "MyBlender.h"
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGTextureProvider>
#include <QSGTextureMaterial>
#include <QDebug>

MyBlender::MyBlender(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

void MyBlender::setSource(QQuickItem *item)
{
    if(source == item)
        return;
    source = item;
    sourceChange = true;
    emit sourceChanged();
    update();
}

QSGNode *MyBlender::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)

    bool abort = false;
    if (!source || !source->isTextureProvider()) {
        qDebug() << "source bg is missing or not a texture provider";
        abort = true;
    }
    if (abort) {
        delete oldNode;
        return nullptr;
    }

    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    if (!oldNode) {
        node = new QSGGeometryNode;

        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip); //GL_TRIANGLE_STRIP
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGTextureMaterial *material = new QSGTextureMaterial;
        material->setTexture(source->textureProvider()->texture());
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        if(sourceChange) {
            sourceChange = false;
            QSGTextureMaterial *material = static_cast<QSGTextureMaterial*>(node->material());
            material->setTexture(source->textureProvider()->texture());
        }
    }

    //QSGGeometry::TexturedPoint2D *vertices = geometry->vertexDataAsTexturedPoint2D();
    //vertices[0].set(0,0,0,0);
    //vertices[1].set(0,0+height(),0,1);
    //vertices[2].set(0+width(),0,1,0);
    //vertices[3].set(0+width(),0+height(),1,1);

    QSGGeometry::updateTexturedRectGeometry(geometry,boundingRect(),QRectF(0, 0, 1, 1));
    node->markDirty(QSGNode::DirtyGeometry);
    node->markDirty(QSGNode::DirtyMaterial);
    return node;
}
