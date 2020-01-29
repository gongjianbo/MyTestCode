#include "FBOItem.h"

#include "FBORenderer.h"
//#include "FBORenderer2.h"

FBOItem::FBOItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    //上下翻转，这样就和OpenGL的坐标系一致了
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *FBOItem::createRenderer() const
{
    //FBORenderer2中使用VAO VBO的方式不能正常缩放，不知道为什么
    return new FBORenderer();
}
