#include "GLView.h"
#include <QCoreApplication>
#include <QDebug>

GLView::GLView(QWidget *parent)
    : QOpenGLWidget(parent)
{
    loadModel(qApp->applicationDirPath()+"/trefoil.obj");

    connect(&timer, &QTimer::timeout, [=]{
        angle += 2;
        update();
    });
    timer.start(100);
}

GLView::~GLView()
{
    if(!isValid())
        return;
    makeCurrent();
    vao.destroy();
    vbo.destroy();
    ebo.destroy();
    doneCurrent();
}

void GLView::loadModel(const QString &filepath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath.toUtf8(), aiProcess_Triangulate |
                                             aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) {
        qDebug()<<__FUNCTION__<<importer.GetErrorString();
        return;
    }else{
        // 以递归方式处理ASSIMP的根节点
        processNode(scene->mRootNode, scene);
    }
}

void GLView::processNode(aiNode *node, const aiScene *scene)
{
    qDebug()<<__FUNCTION__<<"mNumMeshes"<<node->mNumMeshes<<"mNumChildren"<<node->mNumChildren;
    // 处理位于当前节点的每个网格
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // 节点对象仅包含索引用来索引场景中的实际对象。
        // 场景包含所有数据，节点只是为了有组织的保存东西（如节点之间的关系）。
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // 在我们处理完所有网格（如果有的话）后，我们会递归处理每个子节点
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void GLView::processMesh(aiMesh *mesh, const aiScene *scene)
{
    Q_UNUSED(scene)
    qDebug()<<__FUNCTION__<<"mNumVertices"<<mesh->mNumVertices<<"mNumFaces"<<mesh->mNumFaces;
    // 简单的只取顶点坐标和索引
    // QVector<float> vertices;
    // QVector<unsigned int> indices;
    vertices.clear();
    indices.clear();

    // 遍历每个网格的顶点
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices << mesh->mVertices[i].x << mesh->mVertices[i].y << mesh->mVertices[i].z;
    }
    // 现在遍历每个网格面（一个面是一个三角形的网格）并检索相应的顶点索引。
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        if(i == 0) qDebug()<<"mNumFaces"<<face.mNumIndices;
        // 检索面的所有索引并将它们存储在索引向量中
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
}

void GLView::initializeGL()
{
    initializeOpenGLFunctions();

    const char *vertex_str=R"(#version 330 core
layout (location = 0) in vec3 vertices;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
gl_Position = projection * view * model * vec4(vertices, 1.0);
})";
    const char *fragment_str=R"(#version 330 core
out vec4 fragColor;
void main() {
fragColor = vec4(1.0, 0.0, 0.0, 1.0);
})";

    //顶点着色器
    bool success = shaderProgram.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,vertex_str);
    if(!success){
        qDebug()<<"compiler vertex failed!"<<shaderProgram.log();
    }
    //片段着色器
    success = shaderProgram.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,fragment_str);
    if(!success){
        qDebug()<<"compiler fragment failed!"<<shaderProgram.log();
    }
    success = shaderProgram.link();
    if(!success){
        qDebug()<<"link shader failed!"<<shaderProgram.log();
    }

    vao.create();
    vao.bind();

    vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo.create();
    vbo.bind();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(float)); //顶点数据

    ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ebo.create();
    ebo.bind();
    ebo.allocate(indices.data(), indices.size() * sizeof(int)); //索引数据

    //(int location, GLenum type, int offset, int tupleSize, int stride = 0);
    shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(GLfloat) * 3);
    shaderProgram.enableAttributeArray(0);

    vao.release();
    vbo.release();
    ebo.release();

    shaderProgram.bind();
    QMatrix4x4 view; //观察矩阵，后退一点
    view.translate(QVector3D(0.0f, 0.0f, -10.0f));
    shaderProgram.setUniformValue("view", view);
    QMatrix4x4 projection; //透视投影
    projection.perspective(45.0f, 1.0f * width() / height(), 0.1f, 100.0f);
    shaderProgram.setUniformValue("projection", projection);
    shaderProgram.release();
}

void GLView::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 框线模式
    shaderProgram.bind();
    {
        QMatrix4x4 model; //模型矩阵
        model.rotate(angle, QVector3D(0.0f, 1.0f, 0.0f));
        shaderProgram.setUniformValue("model", model);

        QOpenGLVertexArrayObject::Binder vao_bind(&vao); Q_UNUSED(vao_bind);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    shaderProgram.release();
}

void GLView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLView::mousePressEvent(QMouseEvent *event)
{
    QOpenGLWidget::mousePressEvent(event);

    //点击切换模型
    static bool flag = true;
    if(flag){
        loadModel(qApp->applicationDirPath()+"/ball.obj");
    }else{
        loadModel(qApp->applicationDirPath()+"/trefoil.obj");
    }
    flag = !flag;

    vao.bind();
    vbo.bind();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(float)); //顶点数据
    ebo.bind();
    ebo.allocate(indices.data(), indices.size() * sizeof(int)); //索引数据
    shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(GLfloat) * 3);
    shaderProgram.enableAttributeArray(0);

    update();
}
