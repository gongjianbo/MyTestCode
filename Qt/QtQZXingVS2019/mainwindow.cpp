#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QZXing.h"
#include <QImage>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //编码
    connect(ui->btnEncode, &QPushButton::clicked, this, [this](){
        //使用encodeData需要加上宏DEFINES += ENABLE_ENCODER_GENERIC
        img = QZXing::encodeData(ui->textEdit->toPlainText(),
                                 QZXing::EncoderFormat_QR_CODE);
        ui->label->setPixmap(QPixmap::fromImage(img));
    });

    //解码
    connect(ui->btnDecode, &QPushButton::clicked, this, [this](){
        if(img.isNull())
            return;
        QZXing decoder;
        //QR Code二维码
        decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);
        //可选设置，赋值自文档demo
        decoder.setSourceFilterType(QZXing::SourceFilter_ImageNormal);
        decoder.setTryHarderBehaviour(QZXing::TryHarderBehaviour_ThoroughScanning |
                                      QZXing::TryHarderBehaviour_Rotate);
        QString info = decoder.decodeImage(img);
        ui->textEdit->setPlainText("decode:" + info);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

