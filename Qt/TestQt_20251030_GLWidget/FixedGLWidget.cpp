#include "FixedGLWidget.h"
#include <QDebug>

#define DRAW_TRIANGLES 0

FixedGLWidget::FixedGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{

}

FixedGLWidget::~FixedGLWidget()
{

}

void FixedGLWidget::setImage(const QImage &image)
{
    qDebug() << __FUNCTION__ << isValid();
    mImage = image.convertToFormat(QImage::QImage::Format_RGBA8888);
    if (!isValid()) {
        return;
    }

    makeCurrent();
    updateTexture();
    doneCurrent();

    update();
}

void FixedGLWidget::initializeGL()
{
    qDebug() << __FUNCTION__;
    initializeOpenGLFunctions();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &mTextureId);

    updateTexture();
}

void FixedGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
#if DRAW_TRIANGLES
#else
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

void FixedGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

#if DRAW_TRIANGLES
    // 测试三角绘制
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex2f( 0.0f,  0.8f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(-0.8f, -0.8f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex2f( 0.8f, -0.8f);
    glEnd();
#else
    if (mImage.isNull() || mTextureId == 0) return;

    // 保持图像比例
    float imageAspect = float(mImage.width()) / mImage.height();
    float widgetAspect = float(width()) / height();
    float drawWidth, drawHeight;
    if (widgetAspect > imageAspect) {
        drawHeight = height();
        drawWidth  = imageAspect * drawHeight;
    } else {
        drawWidth  = width();
        drawHeight = drawWidth / imageAspect;
    }
    float x0 = (width() - drawWidth) / 2.0f;
    float y0 = (height() - drawHeight) / 2.0f;
    float x1 = x0 + drawWidth;
    float y1 = y0 + drawHeight;

    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x0, y0);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x1, y0);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x1, y1);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x0, y1);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

void FixedGLWidget::updateTexture()
{
    if (mImage.isNull() || mTextureId == 0) return;

    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 mImage.width(), mImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 mImage.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
