#include "ProgramGLWidget.h"
#include <QDebug>

// 着色器代码
#if 0
const char *vertex_str=R"(#version 330 core
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoord;
uniform mat4 projection;
out vec2 texCoord;
void main()
{
gl_Position = projection * vec4(inPos, 0.0, 1.0);
texCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
})";
const char *fragment_str=R"(#version 330 core
uniform sampler2D theTexture;
in vec2 texCoord;
out vec4 fragColor;
void main()
{
fragColor = texture(theTexture, texCoord);
})";
#else
// 兼容opengl es2
const char *vertex_str = R"(
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
attribute vec2 inPos;
attribute vec2 inTexCoord;
uniform mat4 projection;
varying vec2 texCoord;
void main() {
gl_Position = projection * vec4(inPos, 0.0, 1.0);
texCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
})";
const char *fragment_str = R"(
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
uniform sampler2D theTexture;
varying vec2 texCoord;
void main() {
gl_FragColor = texture2D(theTexture, texCoord);
})";
#endif

ProgramGLWidget::ProgramGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{

}

ProgramGLWidget::~ProgramGLWidget()
{
    if (!isValid())
        return;

    makeCurrent();
    vbo.destroy();
    ebo.destroy();
    vao.destroy();
    if (texture) delete texture;
    doneCurrent();
}

void ProgramGLWidget::setImage(const QImage &image)
{
    qDebug() << __FUNCTION__ << isValid();
    cache = image.convertToFormat(QImage::QImage::Format_RGBA8888);
    if (!isValid()) {
        return;
    }

    makeCurrent();
    updateTexture();
    doneCurrent();

    updateProjection(width(), height(), image.width(), image.height());
    update();
}

void ProgramGLWidget::initializeGL()
{
    qDebug() << __FUNCTION__;
    initializeOpenGLFunctions();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    if (!shaderProgram.addCacheableShaderFromSourceCode(
            QOpenGLShader::Vertex, vertex_str)){
        qDebug() << "compiler vertex error" << shaderProgram.log();
    }
    if (!shaderProgram.addCacheableShaderFromSourceCode(
            QOpenGLShader::Fragment, fragment_str)){
        qDebug() << "compiler fragment error" << shaderProgram.log();
    }
    if (!shaderProgram.link()) {
        qDebug() << "link shaderprogram error" << shaderProgram.log();
    }

    float vertices[] = {
        // positions          // texture coords
        1.0f,  1.0f,   1.0f, 1.0f, // top right
        1.0f, -1.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f,  0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    vao.create();
    vao.bind();

    vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo.create();
    vbo.bind();
    vbo.allocate(vertices, sizeof(vertices));

    ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ebo.create();
    ebo.bind();
    ebo.allocate(indices, sizeof(indices));

    // position attribute
    int attr = -1;
    attr = shaderProgram.attributeLocation("inPos");
    shaderProgram.setAttributeBuffer(attr, GL_FLOAT, 0, 2, sizeof(GLfloat) * 4);
    shaderProgram.enableAttributeArray(attr);
    // texture coord attribute
    attr = shaderProgram.attributeLocation("inTexCoord");
    shaderProgram.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 2, 2, sizeof(GLfloat) * 4);
    shaderProgram.enableAttributeArray(attr);

    shaderProgram.bind();
    shaderProgram.setUniformValue("theTexture", 0);
    shaderProgram.release();

    updateTexture();
}

void ProgramGLWidget::resizeGL(int w, int h)
{
    float ratio = devicePixelRatioF();
    glViewport(0, 0, w * ratio, h * ratio);
    if (!texture->isCreated()) return;
    updateProjection(w * ratio, h * ratio, texture->width(), texture->height());
}

void ProgramGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!texture->isCreated()) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    shaderProgram.bind();
    vao.bind();
    shaderProgram.setUniformValue("projection", projection);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    vao.release();
    texture->release();
    shaderProgram.release();
}

void ProgramGLWidget::updateTexture()
{
    if (cache.isNull()) return;

    if (texture) delete texture;
    texture = new QOpenGLTexture(cache, QOpenGLTexture::GenerateMipMaps);
    if (!texture->isCreated()) {
        qDebug() << "Failed to load texture";
        return;
    }

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    cache = QImage();
}

void ProgramGLWidget::updateProjection(int viewW, int viewH, int imageW, int imageH)
{
    if (viewW < 1 || viewH < 1 || imageW < 1 || imageH < 1)
        return;

    projection.setToIdentity();

    float viewRatio = float(viewW) / float(viewH);
    float imageRatio = float(imageW) / float(imageH);

    float scaleX = 1.0f;
    float scaleY = 1.0f;
    // imageRatio比viewRatio小则图片宽度偏小，两边要加padding
    // imageRatio比viewRatio大则图片高度偏小，上下要加padding
    if (viewRatio > imageRatio) {
        scaleX = viewRatio / imageRatio;
    } else {
        scaleY = imageRatio / viewRatio;
    }

    projection.ortho(-scaleX, scaleX, -scaleY, scaleY, -1.0f, 1.0f);
}
