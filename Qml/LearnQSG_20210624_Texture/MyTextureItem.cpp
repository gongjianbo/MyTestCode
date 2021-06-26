#include "MyTextureItem.h"
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGTextureProvider>
#include <QSGTextureMaterial>

MyTextureItem::MyTextureItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    //要渲染的节点需要设置此标志
    setFlag(ItemHasContents, true);
}

void MyTextureItem::setSource(QQuickItem *item)
{
    if(source == item)
        return;
    source = item;
    //变更source后需要重置texture
    sourceChange = true;
    emit sourceChanged();
    update();
}

QSGNode *MyTextureItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    //基本逻辑copy自Qt那几个scene graph的示例
    Q_UNUSED(updatePaintNodeData)

    bool abort = false;
    //未设置纹理源或者该Item不能创建纹理，就不显示
    if (!source || !source->isTextureProvider()) {
        qDebug() << "source is null or not a texture provider";
        abort = true;
    }
    if (abort) {
        delete oldNode;
        return nullptr;
    }

    //一个node包含geometry和material，类似于别的架构中的entity、mesh、material
    //node可以是要渲染的物体，也可以是透明度等
    //geometry定义了网格、顶点、结构等，比如坐标点
    //material定义了填充方式
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    if (!oldNode) {
        //如果只是显示基本的线和纹理，用一个QSGGeometryNode即可
        node = new QSGGeometryNode;

        //使用纹理顶点，4个表示四个角
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
        //绘制模式，设置为连续三角，这样只用四个点就可以表示一个矩形
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip); //GL_TRIANGLE_STRIP
        //添加到node
        node->setGeometry(geometry);
        //此设置表示该节点拥有Geometry实例的所有权，并且会在该节点被销毁或重新设置时将其释放
        node->setFlag(QSGNode::OwnsGeometry);

        //创建纹理
        QSGTextureMaterial *material = new QSGTextureMaterial;
        //设置纹理数据，同一个item的纹理内容更新了会自动触发更新
        //但是更换item后需要重新设置texture
        material->setTexture(source->textureProvider()->texture());
        //添加到node
        node->setMaterial(material);
        //此设置表示该节点拥有Material实例的所有权，并且会在该节点被销毁或重新设置时将其释放
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        //如果item重置了就重新设置纹理
        if(sourceChange) {
            sourceChange = false;
            QSGTextureMaterial *material = static_cast<QSGTextureMaterial*>(node->material());
            material->setTexture(source->textureProvider()->texture());
        }
    }

    QSGGeometry::TexturedPoint2D *vertices = geometry->vertexDataAsTexturedPoint2D();
    //四个顶点，每个顶点为屏幕坐标xy+纹理坐标xy
    //opengl纹理坐标默认左下角为0点，但是QSG默认左上角为0点
    //（注意更新纹理的时候，width和height也要绑定更新）
    vertices[0].set(0,0,0,0);
    vertices[1].set(0,0+height(),0,1);
    vertices[2].set(0+width(),0,1,0);
    vertices[3].set(0+width(),0+height(),1,1);
    //效果同上
    //QSGGeometry::updateTexturedRectGeometry(geometry, boundingRect(), QRectF(0, 0, 1, 1));
    //标记为dirty后才会刷新节点对应内容
    node->markDirty(QSGNode::DirtyGeometry);
    node->markDirty(QSGNode::DirtyMaterial);
    return node;
}
