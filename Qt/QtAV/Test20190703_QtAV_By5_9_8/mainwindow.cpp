#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

using namespace QtAV;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //thePlayer->set
    //qDebug()<<1;
    thePlayer=new AVPlayer(this);
    theAvt=new AVTranscoder(this);
    //qDebug()<<2;
    //QtAV::VideoOutput 退出时异常
    theRender=new GLWidgetRenderer2(this);
    //qDebug()<<3;
    if(!theRender->widget()){
        qDebug()<<"theVideo->widget() null";
    }else{
        qDebug()<<"theVideo->widget() ok";
    }
    thePlayer->setRenderer(theRender);
    //setCentralWidget(theVideo->widget());
    ui->playerLayout->addWidget(theRender->widget());

    connect(ui->btnPlay,&QPushButton::clicked,this,[=](){
        qDebug()<<"play"<<ui->urlEdit->text();
        thePlayer->play(ui->urlEdit->text());
    });
    connect(ui->btnPause,&QPushButton::clicked,this,[=](){
        if(thePlayer->isPaused()){
            qDebug()<<"pause false";
            thePlayer->pause(false);
        }else if(thePlayer->isPlaying()){
            qDebug()<<"pause true";
            thePlayer->pause(true);
        }
        //thePlayer->stepBackward();//播放前一帧并暂停
        //thePlayer->stepForward(); //播放下一帧并暂停
    });
    connect(ui->btnStop,&QPushButton::clicked,this,[=](){
        thePlayer->stop();
    });
    connect(ui->btnSaveBegin,&QPushButton::clicked,this,[=](){
        theAvt->setMediaSource(thePlayer);
        theAvt->setOutputFormat("mp4");
        theAvt->setOutputMedia("C:/Users/zhaozhao/Videos/record.mp4");

        if(!theAvt->createVideoEncoder()){
            return;
        }
        theAvt->videoEncoder()->setCodecName("mpeg4");
        //venc->setCodecName("");
        theAvt->setAsync(true);
        theAvt->start();
    });
    connect(ui->btnSaveEnd,&QPushButton::clicked,this,[=](){
        theAvt->stop();
    });

    connect(ui->btnOther,&QPushButton::clicked,this,[=](){
        qDebug()<<"capture"<<ui->urlEdit->text();
        //截屏
        thePlayer->videoCapture()->setCaptureDir("C:/Users/zhaozhao/Pictures");
        thePlayer->videoCapture()->setSaveFormat("PNG");
        thePlayer->videoCapture()->setCaptureName("capture_test");
        thePlayer->videoCapture()->capture();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
