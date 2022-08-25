#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPainter>
#include <QFileInfo>
#include <QDebug>

#include "EasyAudioContext.h"
#include "EasyAudioDecoder.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->boxFormat->setCurrentIndex(1);
    ui->lineEdit->setText(qApp->applicationDirPath()+"/wav_1ch_11.025K_16bit.wav");

    //选择音频文件
    connect(ui->btnFile,&QPushButton::clicked,this,[this]{
        const QString filepath=QFileDialog::getOpenFileName(this);
        if(!filepath.isEmpty())
            ui->lineEdit->setText(filepath);
    });

    //获取音频信息
    connect(ui->btnInfo,&QPushButton::clicked,this,[this]{
        const QString filepath=ui->lineEdit->text();
        QFileInfo info(filepath);
        if(!info.exists())
            return;

        EasyAudioContext context{filepath};
        if(context.isValid()){
            EasyAudioInfo info=context.getAudioInfo();
            QString info_str=
                    "\nfilepath: "+info.filepath+
                    "\nfilename: "+info.filename+
                    "\nencode: "+info.encode+
                    "\nsampleRate: "+QString::number(info.sampleRate)+" Hz"+
                    "\nchannels: "+QString::number(info.channels)+
                    "\nsampleBit: "+QString::number(info.sampleBit)+" bit"+
                    "\nduration: "+QString::number(info.duration)+" ms"+
                    "\nbitRate: "+QString::number(info.bitRate)+" bps"+
                    "\nsize: "+QString::number(info.size)+" byte"+
                    "\ntype: "+info.type;
            ui->textEdit->append(info_str);
        }
    });

    //转为pcm数据
    connect(ui->btnPcm,&QPushButton::clicked,this,[this]{
        const QString filepath=ui->lineEdit->text();
        QFileInfo info(filepath);
        if(!info.exists())
            return;

        //EasyAudioContext *context=new EasyAudioContext(filepath);
        EasyAudioDecoder decoder;
        EasyAudioParameter param;
        param.channels=ui->spinChannel->value();
        param.sampleFormat=AVSampleFormat(ui->boxFormat->currentIndex());
        param.sampleRate=ui->spinRate->value();
        isS16=(param.sampleFormat==AV_SAMPLE_FMT_S16||param.sampleFormat==AV_SAMPLE_FMT_S16P);

        //测试重入
        pcmData=decoder.toPcmData(filepath,param);
        qDebug()<<"pcm data size"<<pcmData.count();
        //pcmData=decoder.toPcmData(filepath,param);
        //qDebug()<<"redecode pcm data size"<<pcmData.count();
        update();
    });

    //转为wav（pcm）文件
    connect(ui->btnWav,&QPushButton::clicked,this,[this]{
        const QString filepath=ui->lineEdit->text();
        QFileInfo info(filepath);
        if(!info.exists())
            return;

        //EasyAudioContext *context=new EasyAudioContext(filepath);
        EasyAudioDecoder decoder;
        EasyAudioParameter param;
        param.channels=ui->spinChannel->value();
        param.sampleFormat=AVSampleFormat(ui->boxFormat->currentIndex());
        param.sampleRate=ui->spinRate->value();
        //qDebug()<<info.filePath()<<info.fileName()
        //       <<info.absoluteDir()<<info.absoluteFilePath()<<info.absolutePath();
        decoder.transcodeToWav(filepath,filepath+".wav",param);
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
    painter.fillRect(rect(),QColor(50,50,50));

    //只绘制short
    if(pcmData.count()<4||!isS16)
        return;
    painter.setPen(QColor(100,100,100));
    painter.translate(0,height()/2);

    const int length=pcmData.count()/2;
    //只绘制16位的数据
    const short *datas=(const short *)pcmData.constData();
    //点的x间隔
    double xspace=width()/(double)length;
    //绘制采样点步进，测试用的固定值，文件比较大懒得算，测试时不要用大文件就行了
    const int step=1;
    //qDebug()<<"step"<<step;
    for(int i=0;i<length-step;i+=step)
    {
        painter.drawLine(xspace*i,-datas[i]/150,xspace*(i+step),-datas[i+step]/150);
    }
}

