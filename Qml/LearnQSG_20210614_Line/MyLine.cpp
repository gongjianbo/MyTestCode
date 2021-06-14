#include "MyLine.h"
#include <QtQuick/QSGNode>
#include <QtQuick/QSGFlatColorMaterial>

MyLine::MyLine(QQuickItem *parent)
    : QQuickItem(parent)
{
    //对于要渲染的组件，要设置此标志
    setFlag(ItemHasContents, true);
}

QSGNode *MyLine::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    //一个node包含geometry和material，类似于别的架构中的entity、mesh、material
    //node可以是要渲染的物体，也可以是透明度等
    //geometry定义了网格、顶点、结构等，比如坐标点
    //material定义了填充方式
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    //第一次调用时为nullptr，后面每次会把原来的节点指针传进来
    if (!oldNode) {
        //QSGGeometryNode是一个便捷类，用于可渲染节点
        node = new QSGGeometryNode;

        //构造顶点，提供了三种简便设置
        //defaultAttributes_Point2D(); 普通坐标点
        //defaultAttributes_TexturedPoint2D(); 带纹理坐标点
        //defaultAttributes_ColoredPoint2D(); 带颜色坐标点
        //这里设置为了两个坐标点的结构，后面会设置坐标值
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2);
        //线宽
        geometry->setLineWidth(2);
        //绘制模式，setDrawingMode(DrawLines)相当于OpenGL的glDrawArrays(GL_LINES)
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        //添加到node
        node->setGeometry(geometry);
        //此设置表示该节点拥有Geometry实例的所有权，并且会在该节点被销毁或重新设置时将其释放
        node->setFlag(QSGNode::OwnsGeometry);

        //构造材质，这里使用的纯色
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        //颜色
        material->setColor(QColor(255, 0, 0));
        //添加到node
        node->setMaterial(material);
        //此设置表示该节点拥有Material实例的所有权，并且会在该节点被销毁或重新设置时将其释放
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        //初始化完成后，后面的刷新会进入到这个逻辑里
        //这里我们可以更新geometry
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        //可以重置坐标点个数，比如刷新了数据，调用update，就可以在此处重新设置
        //geometry->allocate(2);
    }

    //这个函数是同geometry构造参数对应的，相当于cast了一个内存块的指针
    //如果是自定义的geometry结构，直接geometry->vertexData()拿指针就行
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    vertices[0].set(0,0); //点1左上角
    vertices[1].set(width(),height()); //点2右下角
    //设置dirty标志后场景图才会刷新对应内容
    //如果不设置dirty geometry，那么数据变化就不会刷新
    //（在此例里表现为拖动大小后没有刷新ui渲染）
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
