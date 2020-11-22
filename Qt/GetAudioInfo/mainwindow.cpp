#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPainter>
#include <QDebug>

#include "EasyAudioContext.h"
#include "EasyAudioDecoder.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnFile,&QPushButton::clicked,this,[this]{
        const QString filepath=QFileDialog::getOpenFileName(this);
        ui->lineEdit->setText(filepath);
    });

    connect(ui->btnPcm,&QPushButton::clicked,this,[this]{
        EasyAudioContext *context=new EasyAudioContext(ui->lineEdit->text());
        EasyAudioDecoder decoder=EasyAudioDecoder(context);
        EasyAudioParameter param;
        param.channels=1;
        param.sampleFormat=AV_SAMPLE_FMT_S16;
        param.sampleRate=8000;

        pcmData=decoder.toPcmData(param);
        update();
        qDebug()<<"pcm data size"<<pcmData.count();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    if(pcmData.count()<4)
        return;
    painter.translate(0,height()/2);
    const int length=pcmData.count()/2;
    const short *datas=(const short *)pcmData.constData();
    //点的x间隔
    double xspace=width()/(double)length;
    //绘制采样点步进，测试用的固定值，文件比较大懒得算
    const int step=10;
    //qDebug()<<"step"<<step;
    for(int i=0;i<length-step;i+=step)
    {
        painter.drawLine(xspace*i,-datas[i]/150,xspace*(i+step),-datas[i+step]/150);
    }
}

