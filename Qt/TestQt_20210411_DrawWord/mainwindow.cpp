#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMath>
#include <QMouseEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnClear,&QPushButton::clicked,[this]{
        pointTable.clear();
        pointTemp.clear();
        update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    QPainter painter(this);
    draw(&painter);
}

void MainWindow::draw(QPainter *painter)
{
    //painter->fillRect(this->rect(),QColor(200,220,210));
    //painter->setPen(QPen(QColor(0,255,0),2,Qt::SolidLine,Qt::RoundCap,Qt::BevelJoin));
    painter->setRenderHint(QPainter::Antialiasing,true);
    for(const QVector<QPointF> &points:pointTable)
    {
        drawSeries(painter,points);
    }
    drawSeries(painter,pointTemp);
}

void MainWindow::drawSeries(QPainter *painter, const QVector<QPointF> &points)
{
    if(points.isEmpty())
        return;

    double width_top=1;
    double width_bottom=1;
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    if(points.count()>1){
        QList<QPointF> top_points;
        QList<QPointF> bottom_points;
        double prev_degree=0;
        double prev_distance=0;

        for(int i=0;i<points.count()-1;i++)
        {
            const QPointF &p1=points.at(i);
            const QPointF &p2=points.at(i+1);

            double degree=calcDegree(p1,p2);
            double degree_offset=90*M_PI/180;
            double distance_offset=abs(sqrt(pow(p1.x()-p2.x(),2)+pow(p1.y()-p2.y(),2)));
            if(i==0)
            {
                prev_degree=degree;
                prev_distance=0;
                width_top=distanceToWidth(distance_offset)/2;
                width_bottom=width_top;
                path.addEllipse(points.at(i),width_top,width_bottom);
            }

            if(distance_offset>prev_distance+2){
                distance_offset=prev_distance+2;
            }else if(distance_offset<prev_distance-2){
                distance_offset=prev_distance-2;
            }
            if(i+6>points.count()-1)
            {
                //最后几个点收敛
                double temp=maxWidth-(points.count()-1-i)*(maxWidth/6.0);
                if(distance_offset<temp)
                    distance_offset=temp;
                //qDebug()<<"o"<<distance_offset<<temp;
            }

            QPointF p1_top=QPointF(p1.x()+width_top*cos(degree-degree_offset),
                                   p1.y()+width_top*sin(degree-degree_offset));
            QPointF p1_bottom=QPointF(p1.x()+width_bottom*cos(degree+degree_offset),
                                      p1.y()+width_bottom*sin(degree+degree_offset));

            width_top=distanceToWidth(distance_offset)/2;
            width_bottom=width_top;

            QPointF p2_top=QPointF(p2.x()+width_top*cos(degree-degree_offset),
                                   p2.y()+width_top*sin(degree-degree_offset));
            QPointF p2_bottom=QPointF(p2.x()+width_bottom*cos(degree+degree_offset),
                                      p2.y()+width_bottom*sin(degree+degree_offset));


            top_points<<p1_top<<p2_top;
            bottom_points<<p1_bottom<<p2_bottom;
            /*QPainterPath sub_path;
            sub_path.moveTo(p1_top);
            sub_path.lineTo(p2_top);
            sub_path.lineTo(p2_bottom);
            sub_path.lineTo(p1_bottom);
            sub_path.closeSubpath();
            path.addPath(sub_path);*/

            //拐弯处圆角
            if(abs(distance_offset-prev_distance)>2||abs(prev_degree-degree)>30*M_PI/180){
                path.addEllipse(points.at(i),width_top,width_bottom);
            }
            prev_degree=degree;
            prev_distance=distance_offset;
        }
        //path.addEllipse(points.last(),width_top,width_bottom);
        path.moveTo(top_points.first());
        for(int i=1;i<top_points.count()-1;i++)
        {
            path.lineTo(top_points.at(i));
        }
        path.lineTo(points.last());
        for(int i=bottom_points.count()-2;i>=0;i--)
        {
            path.lineTo(bottom_points.at(i));
        }
        path.closeSubpath();
        //painter->drawPath(path);
        //painter->fillPath(path,Qt::red);
    }
    painter->setPen(QPen(QColor(0,0,0),1));
    //painter->fillPath(path,Qt::red);
    /*QPainterPath path;
    path.moveTo(points.first());
    for(auto pt:points)
        path.lineTo(pt);*/
    //QPainterPathStroker stroker;
    //stroker.setWidth(20);
    //path=stroker.createStroke(path);
    //painter->drawPath(path);
    painter->fillPath(path,Qt::red);
    //painter->drawPath(path);
    painter->setPen(QPen(QColor(0,255,0),3));
    //painter->drawPoints(points.constData(),points.count());
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QMainWindow::mousePressEvent(event);
    qDebug()<<"press"<<event->pos();
    pointTemp.clear();
    pointTemp.push_back(event->pos());
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QMainWindow::mouseReleaseEvent(event);
    qDebug()<<"release"<<event->pos();
    if(pointTemp.count()&&event->pos()!=pointTemp.last()){
        pointTemp.push_back(event->pos());
    }
    pointTable.push_back(pointTemp);
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QMainWindow::mouseMoveEvent(event);
    //qDebug()<<"move"<<event->pos();
    if(pointTemp.count()&&abs(event->pos().x()-pointTemp.last().x())<4&&
            abs(event->pos().y()-pointTemp.last().y())<4){

    }else{
        pointTemp.push_back(event->pos());
        update();
    }
}

double MainWindow::calcDegree(const QPointF &p1, const QPointF &p2)
{
    //角度=atan2(y,x) --atan2式已将象限纳入考量的反正切函数
    //atan2结果是以右侧为0点，顺时针半圆为正，逆时针半圆为负，单位是弧度？
    //需要转换为值正北为0点，顺时针增长，单位转为角度
    //注意这个顺时针是因为y在屏幕坐标系反得
    const double calc_tan=qAtan2(p2.y()-p1.y(),p2.x()-p1.x());
    const double calc_degree=calc_tan;

    //qDebug()<<p1<<p2<<calc_tan<<calc_degree;
    return calc_degree;
}

double MainWindow::distanceToWidth(double distance)
{
    double width=maxWidth-distance;
    if(width>maxWidth){
        width=maxWidth;
    }else if(width<2){
        width=2;
    }
    return width;
}

