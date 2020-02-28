#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //这是固定宽高的演示
    setFixedSize(ImgWidth,ImgHeight);

    //data用alpha叠加
    _dataImg=QImage(ImgWidth,ImgHeight,QImage::Format_Alpha8);
    _dataImg.fill(Qt::transparent);
    //热力图通过alpha值查表
    _heatImg=QImage(ImgWidth,ImgHeight,QImage::Format_ARGB32);
    _heatImg.fill(Qt::transparent);

    //根据线性渐变色条得到颜色表
    QLinearGradient linear=QLinearGradient(QPoint(0,0),QPoint(255,0));
    linear.setColorAt(0, Qt::blue);
    linear.setColorAt(0.4, Qt::blue);
    linear.setColorAt(0.5, Qt::cyan);
    linear.setColorAt(0.6, Qt::green);
    linear.setColorAt(0.8, Qt::yellow);
    linear.setColorAt(0.95, Qt::red);

    //把渐变色绘制到Img方便取颜色
    QImage img(256,1,QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.fillRect(img.rect(),linear);

    //HeatAlpha为热力图整体透明度
    quint32 alpha=0;
    for(quint32 i=0;i<256;i++){
        //根据热力图透明度来计算颜色表的透明度
        alpha=HeatAlpha/255.0*i;
        //typedef unsigned int QRgb: format #AARRGGBB
        //颜色+透明度
        _colorList[i]=img.pixel(i,0)&0x00FFFFFF|(alpha<<24);
    }

    //清空图像
    connect(ui->btnClear,&QPushButton::clicked,this,[=](){
        this->clear();
    });
    //保存图像
    connect(ui->btnSave,&QPushButton::clicked,this,[=](){
        _dataImg.save("save.png","PNG");
    });
}

MainWindow::~MainWindow()
{
    delete []_countTable;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    //绘制一个黑色网格，便于查看渐变色
    p.setPen(QPen(Qt::black,2));
    for(int i=0;i<width();i+=50){
        p.drawLine(i,0,i,height());
    }
    for(int i=0;i<height();i+=50){
        p.drawLine(0,i,width(),i);
    }
    //绘制热力图
    p.drawImage(0,0,_heatImg);
    QMainWindow::paintEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    const QPoint pos=event->pos();
    const int radius=ui->spinBox->value();

    //次数默认为1
    const Point pt{pos.x(),pos.y(),radius,1};
    const int pos_count=appendPoint(pt);
    //目前是根据最大次数来计算的，或许也可以根据次数来分段
    //为什么不直接if(pos_count>_maxCount)才重新绘制？
    //而是有两个点叠加if(pos_count>1)就重新绘制？
    //因为单纯的叠加目前还没有带入权重来计算，如果最大权重更大，那么这个叠加的点颜色就走样了
    if(pos_count>1){
        if(pos_count>_maxCount)
            _maxCount=pos_count;
        drawDataImg();
    }else{
        drawDataPoint(pt);
    }
    drawHeatImg();
    update();
}

int MainWindow::appendPoint(const Point &pt)
{
    //无效的数据
    if(pt.posX<0||pt.posY<0||pt.radius<0||
            pt.posX>=ImgWidth||pt.posY>=ImgHeight)
        return 0;
    //根据权重表获知是否已有该点
    if(_countTable[pt.posX+pt.posY*ImgWidth]>0){
        for(Point &the_pos:_posList)
        {
            if(the_pos.posX==pt.posX&&the_pos.posY==pt.posY){
                //对已有点叠加权重值
                the_pos.count+=pt.count;
                break;
            }
        }
    }else{
        _posList.push_back(pt);
    }
    _countTable[pt.posX+pt.posY*ImgWidth]+=pt.count;
    return _countTable[pt.posX+pt.posY*ImgWidth];
}

void MainWindow::drawDataImg()
{
    //重新绘制先清空
    _dataImg.fill(Qt::transparent);
    QPainter painter(&_dataImg);
    painter.setPen(Qt::transparent);
    //绘制点的部分可以调用drawpoint
    const double max_count=_maxCount;
    for(int i=0;i<_posList.count();i++)
    {
        const Point &pt=_posList.at(i);
        //以最大次数来计算该点的权重
        const uchar alpha=uchar(_countTable[pt.posX+pt.posY*ImgWidth]/max_count*255);
        QRadialGradient gradient(pt.posX,pt.posY,pt.radius);
        gradient.setColorAt(0,QColor(0,0,0,alpha));
        gradient.setColorAt(1,QColor(0,0,0,0));
        painter.setBrush(gradient);
        painter.drawEllipse(QPointF(pt.posX,pt.posY),pt.radius,pt.radius);
    }
}

void MainWindow::drawDataPoint(const Point &pt)
{
    QPainter painter(&_dataImg);
    painter.setPen(Qt::transparent);

    //以最大次数来计算该点的权重
    const uchar alpha=uchar(_countTable[pt.posX+pt.posY*ImgWidth]/(double)_maxCount*255);

    QRadialGradient gradient(pt.posX,pt.posY,pt.radius);
    gradient.setColorAt(0,QColor(0,0,0,alpha));
    gradient.setColorAt(1,QColor(0,0,0,0));
    painter.setBrush(gradient);
    painter.drawEllipse(QPointF(pt.posX,pt.posY),pt.radius,pt.radius);
}

void MainWindow::drawHeatImg()
{
    //把alpha值转为颜色值
    for(int row=0;row<_dataImg.height();row++)
    {
        //dataimg QImage::Format_Alpha8，一个点1个字节
        const uchar *line_data=_dataImg.scanLine(row);
        //heatimg QImage::Format_ARGB32，一个点4个字节
        QRgb *line_heat=reinterpret_cast<QRgb*>(_heatImg.scanLine(row));
        for(int col=0;col<_dataImg.width();col++)
        {
            //根据alpha透明度从颜色表取颜色
            line_heat[col]=_colorList[line_data[col]];
        }
    }
}

void MainWindow::clear()
{
    _dataImg.fill(Qt::transparent);
    _heatImg.fill(Qt::transparent);
    _posList.clear();
    _maxCount=1;
    memset(_countTable,0,ImgWidth*ImgHeight*sizeof(int));
    update();
}

