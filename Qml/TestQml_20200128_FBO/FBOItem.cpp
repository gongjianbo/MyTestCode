#include "FBOItem.h"

#include "FBORenderer.h"
#include "FBORenderer2.h"

FBOItem::FBOItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    //上下翻转，这样就和OpenGL的坐标系一致了
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *FBOItem::createRenderer() const
{
    //Renderer 和 FBO 都是内部管理的内存
    return new FBORenderer();
}
