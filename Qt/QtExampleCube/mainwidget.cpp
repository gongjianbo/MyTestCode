#include "mainwidget.h"
#include <QMouseEvent>
#include <cmath>

MainWidget::~MainWidget()
{
    //QOpenGLWidget
    //三个虚函数不需要makeCurrent，对应的操作已由框架完成
    //但是释放时需要设置当前上下文
    //initializeGL在显示时才调用，释放若未初始化的会导致异常
    //可以在释放前判断：if(!isValid()) { return; }
    makeCurrent();
    //释放纹理数据
    delete texture;
    //释放顶点缓冲
    delete geometries;
    doneCurrent();
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    //保存鼠标按下时的坐标
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    //鼠标按下和释放两点间差值
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
    //旋转轴垂直于鼠标位置差向量，所以x和y交换
    //QVector3D::normalized归一化
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
    //根据拖动距离来计算旋转速度
    qreal acc = diff.length() / 100.0;
    //计算新的旋转轴
    //上一次的旋转还未结束时会叠加到一起
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();
    //加速旋转
    angularSpeed += acc;
}

void MainWidget::timerEvent(QTimerEvent *)
{
    //旋转减速
    angularSpeed *= 0.99;
    //低于某个阈值时停止转动
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        //当前旋转方向向量和之前的状态叠加，计算得到最新的四元数
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
        //刷新ui，重新绘制
        update();
    }
}

void MainWidget::initializeGL()
{
    //为当前上下文初始化OpenGL函数解析
    initializeOpenGLFunctions();
    //设置glClear填充的颜色
    glClearColor(0, 0, 0, 1);

    //初始化着色器程序
    initShaders();
    //初始化纹理
    initTextures();

    //使能深度缓冲GL_DEPTH_TEST
    glEnable(GL_DEPTH_TEST);
    //使能面剔除GL_CULL_FACE
    //glCullFace可以设置GL_FRONT正面剔除或者GL_BACK背面剔除（默认）
    //glFrontFace指定正面的绕序，GL_CCW逆时针（默认）或者GL_CW顺时针
    glEnable(GL_CULL_FACE);

    //初始化顶点缓冲
    geometries = new GeometryEngine;

    //启动定时器，12ms刷新一次
    timer.start(12, this);
}

void MainWidget::initShaders()
{
    //【注意】这里面的close关闭窗口其实并没有实际效用
    //编译顶点着色器，此处作用同：glCreateShader+glShaderSource+glCompileShader+glAttachShader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();
    //编译片元着色器
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();
    //着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本。
    //如果要使用刚才编译的着色器我们必须把它们链接(Link)为一个着色器程序对象，
    //然后在渲染对象的时候激活这个着色器程序。已激活着色器程序的着色器将在我们发送渲染调用的时候被使用。
    //当链接着色器至一个程序的时候，它会把每个着色器的输出链接到下个着色器的输入。
    //当输出和输入不匹配的时候，你会得到一个连接错误。
    //此处作用同：glLinkProgram
    if (!program.link())
        close();
    //激活着色器程序对象，此处作用同：glUseProgram
    //因为后面还需要初始化纹理和顶点
    if (!program.bind())
        close();
}

void MainWidget::initTextures()
{
    //加载一张六个面的精灵图作为纹理，然后使用纹理坐标裁剪对应的区域进行贴图
    //QImage::mirrord默认是上下翻转，因为GL的y轴和屏幕坐标系是相反的
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());
    //指定放大缩小的纹理过滤，GL_NEAREST使用临近插值，GL_LINEAR使用线性插值
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    //纹理环绕方式，当纹理坐标超出默认范围时Repeat重复填充数据
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h)
{
    //计算宽高比
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    //投影参数
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    //重置投影矩阵为单位矩阵
    projection.setToIdentity();
    //透视投影
    //参数fovy定义视野在Y-Z平面的角度，范围是[0.0, 180.0]；
    //参数aspect是投影平面宽度与高度的比率；
    //参数Near和Far分别是近远裁剪面到视点(沿Z负轴)的距离
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL()
{
    //清除颜色和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //绑定纹理
    //着色器程序在初始化时绑定了且未解绑，所以不用再次绑定
    texture->bind();

    //计算modelview矩阵
    QMatrix4x4 matrix;
    //-5离camera远一点，使之能看到cube全貌
    matrix.translate(0.0, 0.0, -5.0);
    //四元数旋转
    matrix.rotate(rotation);

    //设置modelview-projection矩阵，Qt封装的矩阵相乘顺序好像和OpenGL一样的
    program.setUniformValue("mvp_matrix", projection * matrix);
    //激活纹理单元，纹理单元GL_TEXTURE0默认总是被激活
    //program.setUniformValue("texture", 0);
    //绘制顶点数据
    geometries->drawCubeGeometry(&program);
}
