#include "UVCView.h"
#include "HandyTool.h"
#include "HandyConfig.h"
#include <QCoreApplication>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QDateTime>
#include <QDebug>

UVCView::UVCView(QQuickItem *parent)
    : QQuickItem(parent)
    , core{new UVCCore(this)}
{
    setFlag(ItemHasContents, true);
    connect(core, &UVCCore::grabberImageComing,
            this, &UVCView::grabberImageRecv, Qt::QueuedConnection);
}

UVCView::~UVCView()
{

}

UVCCore *UVCView::getCore()
{
    return core;
}

QSGNode *UVCView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGSimpleTextureNode *node = dynamic_cast<QSGSimpleTextureNode *>(oldNode);

    if(!node){
        node = new QSGSimpleTextureNode();
    }

    QSGTexture *m_texture = window()->createTextureFromImage(viewImage);
    node->setTexture(m_texture);
    node->setOwnsTexture(true);

    //按比例缩放并居中
    QRect node_rect = boundingRect().toRect();
    const double image_ratio = viewImage.width() / (double)viewImage.height();
    const double rect_ratio = node_rect.width() / (double)node_rect.height();
    //image_ratio>rect_ratio表示太窄了rect要根绝宽度重新适配高度，否则太矮了重新适配宽度
    if (image_ratio > rect_ratio) {
        const int new_height = node_rect.width() / image_ratio;
        node_rect.setY(node_rect.y() + (node_rect.height() - new_height) / 2);
        node_rect.setHeight(new_height);
    } else {
        const int new_width = image_ratio * node_rect.height();
        node_rect.setX(node_rect.x() + (node_rect.width() - new_width) / 2);
        node_rect.setWidth(new_width);
    }
    node->setRect(node_rect);
    //node->markDirty(QSGNode::DirtyForceUpdate);
    if (sizeChange) {
        node->markDirty(QSGNode::DirtyGeometry);
        sizeChange = false;
    }
    node->markDirty(QSGNode::DirtyMaterial);

    return node;
}

void UVCView::grabberImageRecv(const QImage &image)
{
    if (viewImage.size() != image.size()) {
        sizeChange = true;
    }
    viewImage = image;
    update();
}
