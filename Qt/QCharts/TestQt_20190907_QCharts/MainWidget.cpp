#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , chart(new QChart())
    , yAxis(new QValueAxis())
    , xAxis(new QDateTimeAxis())
    , dataTimer(new QTimer(this))
    //, movingTimer(new QTimer(this))
{
    ui->setupUi(this);

    init();

    //测试用
    appendLine("Test A",QColor(255,0,0));
    appendLine("Test B",QColor(0,255,0));
    appendLine("Test C",QColor(255,255,0));
    connect(dataTimer,&QTimer::timeout,this,[this](){
        //y为随机数
        addData("Test A",qrand()%11-5,QDateTime::currentDateTime());
        addData("Test B",qrand()%21-10,QDateTime::currentDateTime());
        addData("Test C",qrand()%17-8,QDateTime::currentDateTime());
    });

    connect(ui->btnStart,&QPushButton::clicked,this,[this](){
        //设置时间轴范围
        xAxis->setMin(QDateTime::currentDateTime());
        xAxis->setMax(QDateTime::currentDateTime().addSecs(60));
        dataTimer->start(1000);//1s间隔
        //movingTimer->start(100);
    });
    connect(ui->btnStop,&QPushButton::clicked,this,[this](){
        dataTimer->stop();
        //movingTimer->stop();
    });
    /*connect(movingTimer,&QTimer::timeout,this,[this](){
        xAxis->setMin(QDateTime::currentDateTime().addSecs(-60));
        xAxis->setMax(QDateTime::currentDateTime());
    });*/
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::init()
{
    QFont titile_font;
    titile_font.setFamily("SimSun");//宋体
    titile_font.setPixelSize(16); //标题16px
    QFont content_font;
    content_font.setFamily("SimSun");//宋体
    content_font.setPixelSize(12); //内容12px
    //图表设置
    chart->layout()->setContentsMargins(5, 5, 5, 5);//设置外边界
    chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界
    chart->setBackgroundRoundness(0);//设置背景区域无圆角
    chart->setBackgroundVisible(false);
    chart->setTitle("QtCharts Test");
    chart->setTitleBrush(QColor(Qt::white));
    chart->setTitleFont(titile_font);
    chart->setAnimationDuration(1000); //动画时间需要比刷新残影短
    //chart->setAnimationOptions(QChart::SeriesAnimations);//动画可选项
    //图例设置
    chart->legend()->setAlignment(Qt::AlignRight); //放在右侧
    //chart->legend()->setBorderColor(QColor(Qt::white));
    chart->legend()->setLabelColor(QColor(Qt::white)); //图例文字颜色
    chart->legend()->setAcceptHoverEvents(true); //鼠标hover
    setMouseTracking(true);//鼠标hover
    //Y轴设置
    yAxis->setTickCount(5); //刻度线数，不能小于2
    yAxis->setGridLineVisible(true);//网格可见
    yAxis->setGridLinePen(QPen(Qt::gray,1));
    yAxis->setLinePen(QPen(Qt::white)); //刻度线颜色
    yAxis->setLabelFormat("%.1f"); //标签格式
    yAxis->setLabelsFont(content_font); //刻度值字体
    yAxis->setLabelsColor(QColor(Qt::white)); //刻度值颜色
    yAxis->setTitleText("[Y] Value");
    yAxis->setTitleFont(titile_font);
    yAxis->setTitleBrush(QColor(Qt::white));
    chart->addAxis(yAxis,Qt::AlignLeft); //添加到图表
    //X轴设置
    xAxis->setTickCount(7);
    xAxis->setFormat("hh:mm");
    xAxis->setMin(QDateTime::currentDateTime());//设置时间轴范围
    xAxis->setMax(QDateTime::currentDateTime().addSecs(60));
    xAxis->setGridLineVisible(false);//网格可见
    xAxis->setGridLinePen(QPen(Qt::gray,1));
    xAxis->setLinePen(QPen(Qt::white));
    //xAxis->setMinorTickCount(0);
    //xAxis->setMinorGridLineVisible(false);//次要网格可见性
    //xAxis->setShadesPen(Xpen);
    xAxis->setLabelsFont(content_font);
    xAxis->setLabelsColor(QColor(Qt::white));
    xAxis->setTitleText("[X] Time hh:mm");
    xAxis->setTitleFont(titile_font);
    xAxis->setTitleBrush(QColor(Qt::white));
    chart->addAxis(xAxis,Qt::AlignBottom); //添加到图表
    //
    ui->chartView->setChart(chart);
    //开了抗锯齿轴线变模糊，不开抗锯齿series有锯齿，并且若没启用opengl两个状态效率有点差距
    //ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWidget::appendLine(const QString &name, const QColor &color)
{
    if(seriesTable.contains(name))
        return;
    QLineSeries *series=new QLineSeries();
    series->setName(name);
    series->setPen(QPen(color,2));
    QFont font;
    font.setFamily("SimSun");//宋体
    font.setPixelSize(14);
    series->setPointLabelsFont(font);//标签字体
    series->setPointLabelsColor(QColor(Qt::white));//标签颜色
    series->setPointLabelsClipping(true);//标签裁剪
    series->setPointLabelsFormat("@yPoint");//标签格式
    //series->setPointLabelsVisible(true);//显示标签
    series->setPointsVisible(true);//显示点

    chart->addSeries(series);
    //series还有其他信号可供自定义使用
    //设置hover图例的样式
    for(const auto &marker:chart->legend()->markers(series)){
        connect(marker,&QLegendMarker::hovered,this,[=](bool hovered){
            if(hovered){
                series->setPointLabelsVisible(true);
                series->setPen(QPen(color.lighter(),2));
            }else{
                series->setPointLabelsVisible(false);
                series->setPen(QPen(color,2));
            }
        });
    }
    series->attachAxis(yAxis);
    series->attachAxis(xAxis);
    //series->setUseOpenGL(true);//貌似打开后效率高点,但是LineSeries不能显示文本
    seriesTable[name]=series;
}

void MainWidget::removeLine(const QString &name)
{
    if(seriesTable.contains(name)){
        QLineSeries*series=seriesTable.value(name);
        chart->removeSeries(series);
        series->deleteLater();
    }
}

void MainWidget::addData(const QString &name, int value, QDateTime time)
{
    if(seriesTable.contains(name)){
        QLineSeries*series=seriesTable.value(name);
        addData(series,time.toMSecsSinceEpoch(),value);
        //series->append(time.toMSecsSinceEpoch(),value);
    }
}

void MainWidget::addData(const QString &name, const QList<MainWidget::LineData> &datas)
{
    if(seriesTable.contains(name)){
        QLineSeries*series=seriesTable.value(name);
        for(const LineData &data:datas){
            addData(series,data.time.toMSecsSinceEpoch(),data.value);
            //series->append(data.time.toMSecsSinceEpoch(),data.value);
        }
    }
}

void MainWidget::clearData(const QString &name)
{
    if(seriesTable.contains(name)){
        QLineSeries*series=seriesTable.value(name);
        series->clear();
    }
}

void MainWidget::addData(QLineSeries *series, double x, double y)
{   
    //根据最大最小值改变范围
    if(y>yAxis->max()){
        const double range=y-yAxis->min();
        yAxis->setMax(y+range/10);
    }else if(y<yAxis->min()){
        const double range=yAxis->max()-y;
        yAxis->setMin(y-range/10);
    }
    //实时移动模式不需要这一句
    if(x>xAxis->max().toMSecsSinceEpoch()){
        series->clear();
        xAxis->setMin(QDateTime::currentDateTime());//设置时间轴范围
        xAxis->setMax(QDateTime::currentDateTime().addSecs(60));
        //qDebug()<<"count:"<<series->count();
    }
    series->append(x,y);
}


