#include "MyMaterial.h"
#include <QSGMaterialShader>
//#include <QOpenGLContext>
//#include <QOpenGLFunctions>

class MyMaterialShader : public QSGMaterialShader
{
public:
    MyMaterialShader()
    {
        //懒得加两个文件，用vertexShader返回
        //setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/qt-project.org/scenegraph/shaders/vertexcolor.vert"));
        //setShaderSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/qt-project.org/scenegraph/shaders/vertexcolor.frag"));
    }

    const char *vertexShader() const override
    {
        return R"(attribute highp vec4 vertexCoord;
attribute highp vec4 vertexColor;
uniform highp mat4 matrix;
uniform highp float opacity;
varying lowp vec4 color;
void main()
{
    gl_Position = matrix * vertexCoord;
    color = vec4(vertexColor.rgb*vertexColor.a,vertexColor.a) * opacity;
}
)";
    }

    const char *fragmentShader() const override
    {
        return R"(varying lowp vec4 color;
void main()
{
    gl_FragColor = color;
})";
    }

    //刷新的时候会调用此函数，通过判断状态来决定哪些需要更新
    void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override
    {
        Q_UNUSED(newEffect)
        Q_UNUSED(oldEffect)
        //QSG混合模式为glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)，提供了预乘混合
        //FlatColorMaterialShader::updateState中预乘了透明度，但现在我把颜色作为顶点属性
        //所glsl改为：color = vec4(vertexColor.rgb*vertexColor.a,vertexColor.a) * opacity;
        if (state.isOpacityDirty()){
            //颜色的变动也设置opacitydirty
            program()->setUniformValue(opacityId, state.opacity());
            //启用LINE_SMOOTH或者MSAA都会导致横向/竖向直线变胖
            //if(state.context()&&state.context()->functions()){
            //    //state.context()->functions()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //    state.context()->functions()->glEnable(GL_BLEND);
            //    state.context()->functions()->glEnable(GL_LINE_SMOOTH);
            //    state.context()->functions()->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            //}
        }

        if (state.isMatrixDirty())
            program()->setUniformValue(matrixId, state.combinedMatrix());
    }

    //顶点属性名，数组最后一个填nullptr
    //对应geometry的vertexData数据结构
    char const *const *attributeNames() const override
    {
        static const char *const attr[] = { "vertexCoord", "vertexColor", nullptr };
        return attr;
    }

private:
    void initialize() override
    {
        opacityId = program()->uniformLocation("opacity");
        matrixId = program()->uniformLocation("matrix");
    }

private:
    int opacityId;
    int matrixId;
};

MyMaterial::MyMaterial()
    : QSGMaterial()
{
    setFlag(Blending, true);
}

QSGMaterialType *MyMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *MyMaterial::createShader() const
{
    return new MyMaterialShader;
}

int MyMaterial::compare(const QSGMaterial *other) const
{
    const MyMaterial *mate = static_cast<const MyMaterial *>(other);
    return fragColor.rgba() - mate->getColor().rgba();
}

void MyMaterial::setColor(const QColor &color)
{
    fragColor = color;
    //带透明则启用混合
    setFlag(Blending, fragColor.alpha() != 0xff);
}
