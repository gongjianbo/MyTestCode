#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>
#include <QtMath>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);

    //界面上放了两个按钮用来前进后退，切换当前图片
    connect(ui->btnPrev,&QPushButton::clicked,this,&MainWindow::toPrev);
    connect(ui->btnNext,&QPushButton::clicked,this,&MainWindow::toNext);

    //随便搞五个色块image当作图片
    int alpha=255;
    QList<QColor> colors=QList<QColor>{
            QColor(255,0,0,alpha),
            QColor(0,255,0,alpha),
            QColor(0,0,255,alpha),
            QColor(255,255,0,alpha),
            QColor(0,255,255,alpha) };

    for(int i=0;i<colors.size();i++)
    {
        ImageNode node;
        node.img=QImage(200,120,QImage::Format_ARGB32);
        node.img.fill(colors.at(i));
        QPainter p(&node.img);
        p.fillRect(QRectF(10,10,20,20),Qt::gray);
        imageList.append(node);
    }
    calcImagePos();
    calcBtnPath();

    //自动切换
    connect(&swipTimer,&QTimer::timeout,[this]{
        //这里可以判断下是否hover某个图，不切换，略
        toNext();
    });
    swipTimer.start(2000);

    //切换动画
    animation.setTargetObject(this);
    animation.setPropertyName("curIndex");
    animation.setEasingCurve(QEasingCurve::OutQuart);
    animation.setDuration(1000);
    //动画结束后定时切换
    connect(&animation,&QPropertyAnimation::stateChanged,
            this,[this](QAbstractAnimation::State newState, QAbstractAnimation::State oldState){
        oldState;
        if(newState!=QAbstractAnimation::Stopped){
            swipTimer.stop();
        }else{
            swipTimer.start(2000);
        }
    });
}

MainWindow::~MainWindow()
{
    //避免结束程序stop时异常
    swipTimer.disconnect(this);
    animation.disconnect(this);
    swipTimer.stop();
    animation.stop();
    delete ui;
}

double MainWindow::getCurIndex() const
{
    return curIndex;
}

void MainWindow::setCurIndex(double index)
{
    curIndex=index;
    emit curIndexChanged();
    //属性动画设置值会调用该接口，此处计算位置并刷新
    calcImagePos();
    update();
}

bool MainWindow::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::HoverMove:{
        QHoverEvent *hover = static_cast<QHoverEvent*>(e);
        if(hover){
            //放到哪个按钮上就切换到对应图片
            for(int i=0;i<btnList.size();i++)
            {
                if(btnList.at(i).contains(hover->pos()))
                {
                    setIndex=i;
                    animation.stop();
                    animation.setStartValue(getCurIndex());
                    animation.setEndValue((double)setIndex);
                    animation.start();
                    break;
                }
            }
        }
    }
        break;
    default:
        break;
    }
    return QMainWindow::event(e);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),Qt::white);
    if(imageList.isEmpty())
        return;

    //切换两种效果
    if(!ui->checkBox->isChecked())
    {
        painter.save();
        //平移到中心点绘制，便于计算
        painter.translate(width()/2,height()/2);
        for(int i=0;i<imageList.size()&&i<drawList.size();i++)
        {
            const ImageNode &node=imageList.at(drawList.at(i));
            QPointF center=QPointF(node.xf*node.img.width(),
                                   node.zf*30);
            //缩放系数归一化到[0.5,1.0]
            const double scale=0.5+0.25*(node.zf+1);
            QRectF rect=QRectF(0,0,
                               node.img.width()*scale,
                               node.img.height()*scale);
            rect.moveCenter(center);
            painter.drawImage(rect,node.img);
        }
        painter.restore();
    }
    else
    {
        for(int i=0;i<imageList.size()&&i<drawList.size();i++)
        {
            const ImageNode &node=imageList.at(drawList.at(i));
            QPointF center=QPointF(node.xf*node.img.width(),
                                   node.zf*20);
            //缩放系数归一化到[0.5,1.0]
            const double scale=0.5+0.25*(node.zf+1);
            QRectF rect=QRectF(0,0,
                               node.img.width()*scale,
                               node.img.height()*scale);
            rect.moveCenter(rect.topLeft());

            painter.save();
            painter.translate(width()/2,height()/2);
            painter.translate(center);
            QTransform trans;
            const double step=360.0/imageList.size();
            const double degree=step*drawList.at(i)-curIndex*step;
            trans.rotate(-degree,Qt::YAxis);
            painter.setTransform(trans, true);
            painter.drawImage(rect,node.img);
            painter.restore();
        }
    }

    //底部画几个圆圈
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    for(int i=0;i<btnList.size();i++)
    {
        painter.setBrush(i==setIndex?Qt::black:Qt::gray);
        painter.drawEllipse(btnList.at(i));
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    calcBtnPath();
}

void MainWindow::calcImagePos()
{
    if(imageList.isEmpty())
        return;
    drawList.resize(imageList.size());
    //每个图之间的角度间隔
    const double step=360.0/imageList.size();
    //绘制时会平移中心点，所以这里以0.0为中心点计算
    for(int i=0;i<imageList.size();i++)
    {
        ImageNode &node=imageList[i];
        //0度为0，+90度是让当前图z值为1
        const double degree=90+step*i-curIndex*step;
        const double radians=qDegreesToRadians(degree);
        //取反则为顺时针变化
        node.xf=-cos(radians);
        node.zf=sin(radians);
        //存下标用于计算堆叠顺序
        drawList[i]=i;
    }

    //根据z排堆叠顺序
    std::sort(drawList.begin(),drawList.end(),
              [this](int a, int b){
        return imageList.at(a).zf<imageList.at(b).zf;
    });
}

void MainWindow::calcBtnPath()
{
    if(imageList.isEmpty())
        return;
    //按钮也可以根据curIndex值来做过渡动画，略
    btnList.resize(imageList.size());
    //绘制位置的规则自己随便定
    int w=rect().width();
    int h=rect().height();
    //底部画几个圆圈
    int btn_space=20;
    int btn_width=14;
    int bar_width=btn_width*imageList.size()+btn_space*(imageList.size()-1);
    for(int i=0;i<imageList.size();i++)
    {
        btnList[i]=QRectF(w/2-bar_width/2+(btn_space+btn_width)*i,
                          h/2+100,
                          btn_width,btn_width);
    }
}

void MainWindow::toPrev()
{
    animation.stop();
    setIndex--;
    //到头了，就切换到尾巴上，为了动画连续，startvalue也设置到尾巴上
    if(setIndex<0){
        setIndex=imageList.size()-1;
        animation.setStartValue(getCurIndex()+imageList.size());
    }else{
        animation.setStartValue(getCurIndex());
    }

    animation.setEndValue((double)setIndex);
    animation.start();
}

void MainWindow::toNext()
{
    animation.stop();
    setIndex++;
    //到尾了，就切换到头上，为了动画连续，startvalue也设置到头上
    if(setIndex>imageList.size()-1){
        setIndex=0;
        animation.setStartValue(getCurIndex()-imageList.size());
    }else{
        animation.setStartValue(getCurIndex());
    }
    animation.setEndValue((double)setIndex);
    animation.start();
}

