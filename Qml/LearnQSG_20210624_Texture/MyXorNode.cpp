#include "MyXorNode.h"
#include <QDebug>

XorBlendMaterial::XorBlendMaterial()
{
    //rhi暂时不考虑
    //setFlag(SupportsRhiShader);
    setFlag(Blending);
}

QSGMaterialType *XorBlendMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *XorBlendMaterial::createShader() const
{
    if (flags().testFlag(RhiShaderWanted))
        return new XorBlendRhiShader;
    else
        return new XorBlendShader;
}

int XorBlendMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const XorBlendMaterial *other = static_cast<const XorBlendMaterial *>(o);

    if (!state.texture1 || !other->state.texture1)
        return state.texture1 ? 1 : -1;

    if (!state.texture2 || !other->state.texture2)
        return state.texture2 ? -1 : 1;

    if (int diff = state.texture1->comparisonKey() - other->state.texture1->comparisonKey())
        return diff;

    if (int diff = state.texture2->comparisonKey() - other->state.texture2->comparisonKey())
        return diff;

    return 0;
}

XorBlendShader::XorBlendShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex, QLatin1String(":/shaders/xorblender.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment, QLatin1String(":/shaders/xorblender.frag"));
}

void XorBlendShader::initialize()
{
    m_matrix_id = program()->uniformLocation("qt_Matrix");
    m_opacity_id  = program()->uniformLocation("qt_Opacity");
    // The texture units never change, only the textures we bind to them so
    // we set these once and for all here.
    //相当于绑定到了GL_TEXTURE0上，后面设置纹理数据直接操作GL_TEXTURE0
    program()->setUniformValue("uSource1", 0); // GL_TEXTURE0
    program()->setUniformValue("uSource2", 1); // GL_TEXTURE1
}

const char * const *XorBlendShader::attributeNames() const
{
    static char const *const attr[] = { "aVertex", "aTexCoord", nullptr };
    return attr;
}

void XorBlendShader::updateState(const QSGMaterialShader::RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    //QuickItem的updateNode接口每次Item.update才调用，但是此接口每次UI刷新都调用
    qDebug()<<"update state";
    XorBlendMaterial *material = static_cast<XorBlendMaterial *>(newEffect);

    if (state.isMatrixDirty())
        program()->setUniformValue(m_matrix_id, state.combinedMatrix());

    if (state.isOpacityDirty())
        program()->setUniformValue(m_opacity_id, state.opacity());

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // We bind the textures in inverse order so that we leave the updateState
    // function with GL_TEXTURE0 as the active texture unit. This is maintain
    // the "contract" that updateState should not mess up the GL state beyond
    // what is needed for this material.
    //先active再bind，步骤和使用Qt封装的OpenGL使用方式一样
    f->glActiveTexture(GL_TEXTURE1);
    material->state.texture2->bind();
    f->glActiveTexture(GL_TEXTURE0);
    material->state.texture1->bind();
}

XorBlendRhiShader::XorBlendRhiShader()
{
    setShaderFileName(VertexStage, QLatin1String(":/shaders/+qsb/xorblender.vert"));
    setShaderFileName(FragmentStage, QLatin1String(":/shaders/+qsb/xorblender.frag"));
}

bool XorBlendRhiShader::updateUniformData(QSGMaterialRhiShader::RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
{
    bool changed = false;
    QByteArray *buf = state.uniformData();
    Q_ASSERT(buf->size() >= 68);

    if (state.isMatrixDirty()) {
        const QMatrix4x4 m = state.combinedMatrix();
        memcpy(buf->data(), m.constData(), 64);
        changed = true;
    }

    if (state.isOpacityDirty()) {
        const float opacity = state.opacity();
        memcpy(buf->data() + 64, &opacity, 4);
        changed = true;
    }

    return changed;
}

void XorBlendRhiShader::updateSampledImage(QSGMaterialRhiShader::RenderState &state, int binding, QSGTexture **texture, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
{
    Q_UNUSED(state);

    XorBlendMaterial *mat = static_cast<XorBlendMaterial *>(newMaterial);
    switch (binding) { // the binding for the sampler2Ds in the fragment shader
    case 1:
        *texture = mat->state.texture1;
        break;
    case 2:
        *texture = mat->state.texture2;
        break;
    default:
        return;
    }
}

XorNode::XorNode(QSGTextureProvider *p1, QSGTextureProvider *p2)
    : m_provider1(p1)
    , m_provider2(p2)
{
    setFlag(QSGNode::UsePreprocess, true);

    // Set up material so it is all set for later..
    m_material = new XorBlendMaterial;
    m_node.setMaterial(m_material);
    m_node.setFlag(QSGNode::OwnsMaterial);

    // Set up geometry, actual vertices will be initialized in updatePaintNode
    //4个顶点表示绘制一个矩形，使用GL_TRIANGLE_STRIP
    m_node.setGeometry(new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4));
    m_node.setFlag(QSGNode::OwnsGeometry);

    // If this node is used as in a shader effect source, we need to propegate
    // changes that will occur in this node outwards.
    connect(m_provider1.data(), &QSGTextureProvider::textureChanged, this, &XorNode::textureChange, Qt::DirectConnection);
    connect(m_provider2.data(), &QSGTextureProvider::textureChanged, this, &XorNode::textureChange, Qt::DirectConnection);
}

void XorNode::preprocess()
{
    //设置了UsePreprocess后，每次刷新前会调用此函数
    //QuickItem的updateNode接口每次Item.update才调用，但是此接口每次UI刷新都调用
    qDebug()<<"preprocess";
    // Update the textures from the providers, calling into QSGDynamicTexture if required
    if (m_provider1) {
        m_material->state.texture1 = m_provider1->texture();
        if (QSGDynamicTexture *dt1 = qobject_cast<QSGDynamicTexture *>(m_material->state.texture1))
            dt1->updateTexture();
    }
    if (m_provider2) {
        m_material->state.texture2 = m_provider2->texture();
        if (QSGDynamicTexture *dt2 = qobject_cast<QSGDynamicTexture *>(m_material->state.texture2))
            dt2->updateTexture();
    }

    // Remove node from the scene graph if it is there and either texture is missing...
    if (m_node.parent() && (!m_material->state.texture1 || !m_material->state.texture2))
        removeChildNode(&m_node);

    // Add it if it is not already there and both textures are present..
    else if (!m_node.parent() && m_material->state.texture1 && m_material->state.texture2)
        appendChildNode(&m_node);
}

void XorNode::setRect(const QRectF &rect)
{
    // Update geometry if it has changed and mark the change in the scene graph.
    if (m_rect != rect) {
        m_rect = rect;
        QSGGeometry::updateTexturedRectGeometry(m_node.geometry(), m_rect, QRectF(0, 0, 1, 1));
        m_node.markDirty(QSGNode::DirtyGeometry);
    }
}

void XorNode::textureChange()
{
    // When our sources change, we will look different, so signal the change to the
    // scene graph.
    markDirty(QSGNode::DirtyMaterial);
}
