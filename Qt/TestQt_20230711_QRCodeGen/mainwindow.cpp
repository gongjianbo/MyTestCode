#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include "qrcodegen.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnGen, &QPushButton::clicked, this, &MainWindow::generate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generate()
{
    // 使用utf8编码
    QByteArray str = ui->lineEdit->text().toUtf8();
    const char *text = str.constData();

    // 二维码有四个纠错等级，从低到高：L-%7/M-%15/Q-%25/H-%30
    const qrcodegen::QrCode::Ecc level = qrcodegen::QrCode::Ecc::MEDIUM;

    // 生成二维码
    try
    {
        // 使用简易接口，默认 Version 范围 min=1, max=40
        // const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text, level);
        // 也可以指定符号版本，越大可容纳的信息越多，如果内容超出容量范围会抛异常
        // 根据规范，Version1 是 21x21，Version2 是 25x25，每增加一个Version，就比前一版本每边增加 4 个模块
        std::vector<qrcodegen::QrSegment> segs = qrcodegen::QrSegment::makeSegments(text);
        const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeSegments(segs, level, 1, 40);
        const int size = qr.getSize();
        // 填充位图
        QImage image = QImage(size, size, QImage::Format_Grayscale8);
        for (int row = 0; row < size; ++row)
        {
            uchar *line_ptr = image.scanLine(row);
            for (int col = 0; col < size; ++col)
            {
                line_ptr[col] = (uchar)(qr.getModule(row, col) ? 0x00 : 0xFF);
            }
        }
        // 放大一点看得更清楚
        image = image.scaled(image.width() * 5, image.height() * 5);

        // 生成后可以用手机扫一扫识别文字内容，注意内容为空可能扫不出来
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
    catch(std::invalid_argument e)
    {
        // 参数异常，如 minVersion > maxVersion
        qDebug() << "catch invalid_argument" << e.what();
    }
    catch(qrcodegen::data_too_long e)
    {
        // 内容太长
        qDebug() << "catch data_too_long" << e.what();
    }
}

