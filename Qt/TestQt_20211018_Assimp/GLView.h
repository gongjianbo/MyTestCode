#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QTimer>

#include "assimp/assimp/Importer.hpp"
#include "assimp/assimp/scene.h"
#include "assimp/assimp/postprocess.h"

// 参照LearnOpenGL：https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/#3dopengl
class GLView :
        public QOpenGLWidget,
        protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLView(QWidget *parent = nullptr);
    ~GLView();

    // 加载模型
    void loadModel(const QString &filepath);
    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);

protected:
    // 渲染需要实现的三个虚函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    // 点击切换模型
    void mousePressEvent(QMouseEvent *event) override;

private:
    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ebo;

    // 定时器旋转模型矩阵
    QTimer timer;
    int angle = 0;

    // 存顶点和索引
    QVector<float> vertices;
    QVector<unsigned int> indices;
};

