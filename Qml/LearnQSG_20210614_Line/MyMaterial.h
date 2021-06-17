#pragma once
#include <QSGMaterial>
#include <QColor>

//Qt部分demo使用了QSGSimpleMaterial，但是此类已标记为obsolete
//所以这里参照源码QSGFlatColorMaterial，以及vertexcolormaterial和texturematerial
//qt-everywhere-src\qtdeclarative\src\quick\scenegraph\util\qsgflatcolormaterial.h
class MyMaterial : public QSGMaterial
{
public:
    MyMaterial();
    //该函数由场景图调用以返回每个子类的唯一实例
    QSGMaterialType *type() const override;
    //对于场景图中存在的每种材质类型，createShader只会被调用一次，并将在内部缓存
    QSGMaterialShader *createShader() const override;
    //将此材料与其他材料进行比较，如果相等则返回0
    //此material先排序为-1，其他material先排序为1.
    //场景图可以重新排列几何节点以最小化状态变化。
    //在排序过程中调用 compare 函数，以便可以对材质进行排序，
    //以尽量减少每次调用QSGMaterialShader::updateState () 时的状态变化。
    int compare(const QSGMaterial *other) const override;

    //自定义接口用于设置材质属性
    const QColor &getColor() const { return fragColor; }
    void setColor(const QColor &color);

private:
    QColor fragColor = QColor(0,0,0);
};
