#pragma once
//参照Qt源码和stackoverflow，效果不是很好
//https://stackoverflow.com/questions/28125425/how-to-smooth-the-painting-of-a-custom-qml-element#
//同时也可以参照示例graph
#include <QQuickItem>
#include <QSGMaterial>
#include <QSGMaterialShader>

//qt-everywhere-src-5.15.2\qtdeclarative\src\quick\scenegraph\qsgdefaultinternalrectanglenode.cpp
class SmoothMaterialShader : public QSGMaterialShader
{
public:
    SmoothMaterialShader(){
        setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/qt-project.org/scenegraph/shaders/smoothcolor.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/qt-project.org/scenegraph/shaders/smoothcolor.frag"));
    }
    void updateState(const RenderState &state,
                     QSGMaterial *newEffect,
                     QSGMaterial *oldEffect) override{
        Q_UNUSED(newEffect)

        if (state.isOpacityDirty())
            program()->setUniformValue(opacityLoc, state.opacity());

        if (state.isMatrixDirty())
            program()->setUniformValue(matrixLoc, state.combinedMatrix());

        if (oldEffect == 0) {
            // The viewport is constant, so set the pixel size uniform only once.
            QRect r = state.viewportRect();
            program()->setUniformValue(pixelSizeLoc, 2.0f / r.width(), 2.0f / r.height());
        }
    }
    char const *const *attributeNames() const override{
        static char const *const attributes[] = {
            "vertex",
            "vertexColor",
            "vertexOffset",
            nullptr
        };
        return attributes;
    }
private:
    void initialize(){
        matrixLoc = program()->uniformLocation("matrix");
        opacityLoc = program()->uniformLocation("opacity");
        pixelSizeLoc = program()->uniformLocation("pixelSize");
    }
    int matrixLoc;
    int opacityLoc;
    int pixelSizeLoc;
};

//qt-everywhere-src-5.15.2\qtdeclarative\src\quick\scenegraph\qsgdefaultinternalrectanglenode_p.h
class SmoothMaterial : public QSGMaterial
{
public:
    SmoothMaterial(){
        setFlag(RequiresFullMatrixExceptTranslate, true);
        setFlag(Blending, true);
    }
    int compare(const QSGMaterial *other) const{
        Q_UNUSED(other)
        // all state in vertex attributes -> all smoothcolor materials are equal
        return 0;
    }
protected:
    QSGMaterialType *type() const override{
        static QSGMaterialType type;
        return &type;
    }
    QSGMaterialShader *createShader() const override{
        return new SmoothMaterialShader;
    }
};

//qt-everywhere-src-5.15.2\qtdeclarative\src\quick\scenegraph\qsgbasicinternalrectanglenode.cpp
//QSGBasicInternalRectangleNode::updateGeometry()
class SmoothLine : public QQuickItem
{
    Q_OBJECT
public:
    explicit SmoothLine(QQuickItem *parent = nullptr);

    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) override;
};
