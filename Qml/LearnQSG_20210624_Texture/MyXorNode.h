#pragma once
#include <QPointer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSGMaterial>
#include <QSGTexture>
#include <QSGGeometryNode>
#include <QSGTextureProvider>

//5.12-5.15-6.1的twotextureproviders示例material、shader皆有不同
//5.12 XorBlendShader继承自public QSGSimpleMaterialShader<>类
//     没有单独的Material类
//5.15 XorBlendMaterial继承自QSGMaterial
//     XorBlendShader、XorBlendRhiShader继承自QSGMaterialShader
//6.1  去掉了XorBlendShader只保留了XorBlendRhiShader，着色器的版本也提高了
//下列代码复制自5.15版本
class XorBlendMaterial : public QSGMaterial
{
public:
    XorBlendMaterial();
    QSGMaterialType *type() const override;
    QSGMaterialShader *createShader() const override;
    int compare(const QSGMaterial *other) const override;

    struct {
        QSGTexture *texture1 = nullptr;
        QSGTexture *texture2 = nullptr;
    } state;
};

class XorBlendShader : public QSGMaterialShader // for when the scenegraph is using OpenGL directly
{
public:
    XorBlendShader();
    void initialize() override;
    char const *const *attributeNames() const override;
    void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;

private:
    int m_matrix_id;
    int m_opacity_id;
};

class XorBlendRhiShader : public QSGMaterialRhiShader // for when the scenegraph is using QRhi
{
public:
    XorBlendRhiShader();
    bool updateUniformData(RenderState &state,
                           QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;
    void updateSampledImage(RenderState &state, int binding, QSGTexture **texture,
                            QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;
};

class XorNode : public QObject, public QSGNode
{
    Q_OBJECT
public:
    XorNode(QSGTextureProvider *p1, QSGTextureProvider *p2);
    void preprocess() override;
    void setRect(const QRectF &rect);

public slots:
    void textureChange();

private:
    QRectF m_rect;
    XorBlendMaterial *m_material;
    QSGGeometryNode m_node;
    QPointer<QSGTextureProvider> m_provider1;
    QPointer<QSGTextureProvider> m_provider2;
};
