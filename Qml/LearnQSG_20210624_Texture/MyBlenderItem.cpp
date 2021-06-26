#include "MyBlenderItem.h"
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGTextureProvider>
#include <QSGTextureMaterial>
#include "MyXorNode.h"

MyBlenderItem::MyBlenderItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

void MyBlenderItem::setSource1(QQuickItem *item)
{
    if(source1 == item)
        return;
    source1 = item;
    source1Change = true;
    emit source1Changed();
    update();
}

void MyBlenderItem::setSource2(QQuickItem *item)
{
    if(source2 == item)
        return;
    source2 = item;
    source2Change = true;
    emit source2Changed();
    update();
}

QSGNode *MyBlenderItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)

    // Check if our input is valid and abort if not, deleting the old node.
    bool abort = false;
    if (!source1 || !source1->isTextureProvider()) {
        qDebug() << "source1 is missing or not a texture provider";
        abort = true;
    }
    if (!source2 || !source2->isTextureProvider()) {
        qDebug() << "source2 is missing or not a texture provider";
        abort = true;
    }
    if (abort) {
        delete oldNode;
        return nullptr;
    }

    XorNode *node = static_cast<XorNode *>(oldNode);

    // If the sources have changed, recreate the nodes
    if (source1Change || source2Change) {
        delete node;
        node = nullptr;
        source1Change = false;
        source2Change = false;
    }

    // Create a new XorNode for us to render with.
    if (!node)
        node = new XorNode(source1->textureProvider(), source2->textureProvider());

    // Update the geometry of the node to match the new bounding rect
    node->setRect(boundingRect());

    return node;
}

