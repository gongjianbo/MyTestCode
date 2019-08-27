#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QtMath>
#include <QTime>
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    //开启悬停事件
    setAttribute(Qt::WA_Hover,true);

    init();

    //添加按钮
    connect(ui->btnAppend,&QPushButton::clicked,this,[this](){
        appendSlice(PieSlice{ui->editName->text(),ui->editValue->text().toDouble()});
    });
    //清除按钮
    connect(ui->btnClear,&QPushButton::clicked,this,[this](){
        clearSlice();
    });
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::init()
{
    //随机数种子
    qsrand(QTime::currentTime().msec()+QTime::currentTime().second()*1000);
    //hover标志
    hoveredIndex=-1;
    hoveredFlag=false;
    //
    pieMargin=20;
    pieCenterPos=QPoint(width()/2,height()/2);
    pieRadius=(width()<height())?(width()/2-pieMargin):(height()/2-pieMargin);
    pieRect=QRectF(pieCenterPos.x()-(pieRadius+pieMargin),pieCenterPos.y()-(pieRadius+pieMargin),
                   (pieRadius+pieMargin)*2,(pieRadius+pieMargin)*2);
    pieValueCount=0;
    //动画状态
    animationProgress=0;
    animationIndexProgress=0;
    animationIndex=0;
    animation.setTargetObject(this);
    animation.setPropertyName("pieProgress");
    animation.setDuration(1000);
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);

    //添加测试数据
    appendSlice(PieSlice("ten_yes",10));
    appendSlice(PieSlice("five_test",5));
    appendSlice(PieSlice("nine_string",9));
    appendSlice(PieSlice("one_punch_man",1));
}

void MainWidget::appendSlice(const PieSlice &slice)
{
    //添加到容器
    pieSliceList.append(slice);
    //计算值的总和
    pieValueCount+=slice.itemValue;

    //降序排列
    std::sort(pieSliceList.begin(),pieSliceList.end(),
              [](const PieSlice&left,const PieSlice&right)->bool{
        return left.itemValue>right.itemValue;
    });
    //本来想增加一个整体的起始角度，但是后面hover时角度不好换算
    double start_angle=0; //起始角度temp，用于累加
    int h_value=-1000; //色度temp，用于相近色计算
    for(PieSlice &item:pieSliceList){
        item.percentage=item.itemValue/pieValueCount; //计算百分比
        item.startAngle=start_angle; //起始角
        item.angleSpan=item.percentage*360; //占的角度值
        start_angle+=item.angleSpan;
        //我是用的随机颜色，也可以通过传入一个颜色列表来取对应颜色
        int new_h_value=qrand()%360;
        //本来想算一个不相近的颜色，但是0附近和359附近颜色也接近
        //并且，没有考虑整体的颜色独立性
        while(qAbs(new_h_value-h_value)<50){
            new_h_value=qrand()%360;
        }
        //Hue 色度[0,359], Lightness 亮度[0,255], Saturation 饱和度[0,255]
        item.color=QColor::fromHsl(new_h_value,220,80);
        h_value=new_h_value; //用于下次计算色度相近
    }
    //update
    animation.start();
}

void MainWidget::clearSlice()
{
    //hover标志
    hoveredIndex=0;
    hoveredFlag=false;
    //清除数据
    pieSliceList.clear();
    pieValueCount=0;
    //动画标志
    animationIndex=0;
    animationIndexProgress=0;
    //清空后刷新
    update();
}

void MainWidget::mouseMove(const QPoint &pos)
{
    //不在范围内则清除hover标志
    if(!pieRect.contains(pos)){
        mouseLeave();
        return;
    }

    mousePos=pos;
    //计算当前所在角度
    const double arc_tan=qAtan2(pos.y()-pieCenterPos.y(),pos.x()-pieCenterPos.x());
    //aten2结果是以右侧为0点，顺时针半圆为正，逆时针半圆为负，单位是弧度？
    //需要转换为值正北为0点，顺时针增长，单位转为角度
    double arc_pos=arc_tan*180/M_PI;
    if(arc_pos<0){
        arc_pos=-arc_pos;
    }else if(arc_pos>0){
        arc_pos=360-arc_pos;
    }

    //计算hover选中的index
    int index=0;
    for(const PieSlice &item:pieSliceList)
    {
        if(arc_pos>=item.startAngle&&arc_pos<=(item.startAngle+item.angleSpan)){
            if(index!=hoveredIndex){
                hoveredIndex=index;
            }
            break;
        }
        ++index;
    }
    //因为由一个tip跟随鼠标移动，所以每次move都update
    hoveredFlag=true;
    update();
}

void MainWidget::mouseLeave()
{
    if(hoveredFlag){
        hoveredFlag=false;
        update();
    }
}

double MainWidget::getPieProgress() const
{
    return animationProgress;
}

void MainWidget::setPieProgress(double pro)
{
    //根据动画进度[0,1]计算相应的slice的index和进度值
    animationProgress=pro;
    const double item_width=1.0/pieSliceList.count();
    animationIndex=pro/item_width;
    animationIndexProgress=(pro-animationIndex*item_width)/item_width;
    update();
}

bool MainWidget::event(QEvent *event)
{
    switch (event->type()) {
    //拖动
    case QEvent::MouseMove:
        mouseMove(static_cast<QMouseEvent*>(event)->pos());
        return true;
        break;
        //滑动
    case QEvent::HoverMove:
        mouseMove(static_cast<QHoverEvent*>(event)->pos());
        return true;
        break;
        //离开
    case QEvent::HoverLeave:
        mouseLeave();
        return true;
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(0,0,width(),height(),QBrush(QColor("black")));

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);

    //绘制扇形，这一部分可以和后面的tip部分分离开，
    //因为扇形变化更少，但是tip只要鼠标move了就要去update，
    //可以先把扇形绘制到一个pixmap中
    painter.save();
    painter.translate(pieCenterPos); //中心点移动到pie的中心点
    painter.setPen(QColor("white"));//绘制文本，这里没有设置字体，请自行设置
    int index=0;
    for(const PieSlice &item:pieSliceList){
        //根据动画进度计算半径当前百分比值
        const double progress=(index<animationIndex)
                ?1
               :(index>animationIndex)
                 ?0
                :animationIndexProgress;
        //半径随index递减样式
        const int radius=(pieRadius-index*5)*progress;
        //slice扇形路径
        QPainterPath path;
        path.moveTo(QPointF(0,0));
        //hover选中时半径突出一点
        if(hoveredFlag&&index==hoveredIndex){
            const QRectF hover_rect=QRectF(-(radius+10),-(radius+10),
                                           (radius+10)*2,(radius+10)*2);
            path.arcTo(hover_rect,item.startAngle,item.angleSpan);
        }else{
            const QRectF pie_rect=QRectF(-radius,-radius,radius*2,radius*2);
            path.arcTo(pie_rect,item.startAngle,item.angleSpan);
        }
        path.lineTo(QPointF(0,0));
        painter.fillPath(path,QBrush(item.color));
        //根据扇形中心点绘制文本（1/2我替换为0.6了）
        const QString text_percent=QString::number(item.percentage*100,'f',2)+"%";
        const double text_angle=item.startAngle+item.angleSpan/2; //span中心
        const int text_height=painter.fontMetrics().height()+2; //加行间隔2
        const int text_namewidth=painter.fontMetrics().width(item.itemName); //名称str宽度
        const int text_percentwidth=painter.fontMetrics().width(text_percent); //值str宽度
        const double text_x=0+radius*0.6*qCos(text_angle/180*M_PI); //文本中心点
        const double text_y=0-radius*0.6*qSin(text_angle/180*M_PI); //文本中心点

        //y轴qt是上负下正，所以加减操作反过来了
        painter.drawText(text_x-text_namewidth/2,text_y,item.itemName);
        painter.drawText(text_x-text_percentwidth/2,text_y+text_height,text_percent);
        ++index;
    }
    painter.restore();

    //绘制选中slice的tip，主要是计算坐标
    //把数据写在一个矩形框上
    if(hoveredFlag&&pieSliceList.count()>hoveredIndex&&0<=hoveredIndex){
        const int rect_margin=5; //矩形边距
        const PieSlice &item=pieSliceList.at(hoveredIndex);
        const QString str_name=item.itemName;
        const int name_width=painter.fontMetrics().width(str_name)+rect_margin*2;
        const QString str_value=QString("value:%1(%2%)")
                .arg(QString::number(item.itemValue,'f',0))
                .arg(QString::number(item.percentage*100,'f',2));
        const int text_height=painter.fontMetrics().height();
        const int value_width=painter.fontMetrics().width(str_value)+rect_margin*2;
        const int rect_height=text_height*2+rect_margin*2+2; //两行+间隔2
        const int rect_width=name_width>value_width?name_width:value_width;
        //左上角坐标，避免超出范围所以要判断并set
        QPointF top_left(mousePos.x()-rect_width,mousePos.y()-rect_height);
        if(top_left.x()<pieRect.x())
            top_left.setX(pieRect.x());
        if(top_left.y()<pieRect.y())
            top_left.setY(pieRect.y());
        //半透明矩形背景，可以fillpath绘制圆角矩形
        painter.fillRect(QRectF(top_left.x(),top_left.y(),rect_width,rect_height),
                         QBrush(QColor(150,150,150,120)));
        painter.setPen(QColor("white"));//绘制文本，这里没有设置字体，请自行设置
        painter.drawText(top_left.x()+rect_margin,
                         top_left.y()+rect_margin+text_height,
                         str_name);
        painter.drawText(top_left.x()+rect_margin,
                         top_left.y()+rect_margin+text_height*2+2,
                         str_value);
    }
    return QWidget::paintEvent(event);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    //尺寸变化的时候中心和半斤等也重新计算
    pieCenterPos=QPoint(width()/2,height()/2);
    pieRadius=(width()<height())?(width()/2-pieMargin):(height()/2-pieMargin);
    pieRect=QRectF(pieCenterPos.x()-(pieRadius+pieMargin),pieCenterPos.y()-(pieRadius+pieMargin),
                   (pieRadius+pieMargin)*2,(pieRadius+pieMargin)*2);
    return QWidget::resizeEvent(event);
}
