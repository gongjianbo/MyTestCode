#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QImage>

// 测试固定管线
class FixedGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit FixedGLWidget(QWidget *parent = nullptr);
    ~FixedGLWidget();

    void setImage(const QImage &image);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void updateTexture();

private:
    // 纹理id
    GLuint mTextureId{0};
    // 纹理图像
    QImage mImage;
};
