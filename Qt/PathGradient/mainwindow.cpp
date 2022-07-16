#include "mainwindow.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QVector2D>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Path && Gradient");
    resize(400, 300);

    //定时器更新颜色偏移
    QTimer *timer =new QTimer(this);
    connect(timer,&QTimer::timeout,this,[this]{
        offset++;
        offset %= 100;
        update();
    });
    timer->start(50);
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter p(this);
    p.fillRect(rect(),Qt::black);

    //将圆弧和贝塞尔曲线添加到路径中
    QRect rt=QRect(100,50,200,200);
    QPainterPath path;
    path.moveTo(300,150);
    path.arcTo(rt,0,-250);
    path.moveTo(50,150);
    path.cubicTo(150,50,250,250,350,150);

    //颜色表，先绘制到img，再根据坐标取颜色值
    QLinearGradient gradient(QPointF(0,0),QPointF(0,100));
    gradient.setColorAt(0,Qt::red);
    gradient.setColorAt(0.333,Qt::green);
    gradient.setColorAt(0.666,Qt::blue);
    gradient.setColorAt(1,Qt::red);
    QImage img(1,100,QImage::Format_ARGB32);
    QPainter imgp(&img);
    imgp.fillRect(img.rect(),gradient);

    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(16);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing);

    //根据路径长度和分片长度来取路径百分比坐标和颜色
    //注意：pointAtPercent比较耗时，可以在resizeEvent等地方计算好坐标点
    double len = path.length();
    int step = 6;
    QPointF pt1=path.pointAtPercent(0);
    QPointF pt2=pt1;
    for(int i=0;i<len;i+=step)
    {
        //根据分片长度获取两端的坐标
        pt1=pt2;
        int i2 = i+step;
        pt2=path.pointAtPercent(i2<len?(i2/len):1.0);
        //两点距离过长说明是到了另一段子路径上
        //那就往前找当前子路径的末尾
        while(QVector2D(pt1).distanceToPoint(QVector2D(pt2))>step+1){
            i2--;
            pt2=path.pointAtPercent(i2<len?(i2/len):1.0);
        }
        //从颜色表取对应的颜色值
        QColor color1=img.pixelColor(0,(i/step+offset)%100);
        QColor color2=img.pixelColor(0,((i+step)/step+offset)%100);
        QLinearGradient lg(pt1,pt2);
        lg.setColorAt(0,color1);
        lg.setColorAt(1,color2);
        //根据两端颜色设置渐变，然后填充该分片
        pen.setBrush(lg);
        p.setPen(pen);
        p.drawLine(pt1,pt2);
        //下一个子路径的端点
        if(i2!=i+step){
            i2++;
            pt2=path.pointAtPercent(i2<len?(i2/len):1.0);
        }
    }
}
