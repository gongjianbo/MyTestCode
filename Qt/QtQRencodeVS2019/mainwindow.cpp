#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qrencode.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, [this](){
        QImage img = qrEncode(ui->edit->toPlainText());
        ui->label->setPixmap(QPixmap::fromImage(img));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * 从字符串创建一个符号。库自动解析输入字符串并在二维码符号中编码.
 * @warning 禁用pthread时线程不安全.
 * @param string NUL('\0')结尾的C字符串.
 * @param version 符号版本.越大可容纳的信息越多.0则按实际内容确定
 * @param level 纠错等级，枚举.
 * @param hint 编码模式，utf8用QR_MODE_8.
 * @param casesensitive 区分大小写(1) 不区分(0).
 * @return 返回QRcode类的实例。结果QRcode的版本可能是大于指定的版本.
 * 出现错误时，返回NULL，设置errno以指示错误.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw ERANGE input data is too large.
 */
//extern QRcode *QRcode_encodeString(const char *string, int version, QRecLevel level,
//                                   QRencodeMode hint, int casesensitive);
QImage MainWindow::qrEncode(const QString &info)
{
    QImage ret; //放二维码图片结果
    int scale = 4; //方块绘制大小
    QByteArray info_data = info.toUtf8();
    QRcode* qr = QRcode_encodeString(info_data.constData(), 0, QR_ECLEVEL_Q, QR_MODE_8, 1);
    if (qr && qr->width > 0)
    {
        int img_width = qr->width * scale;
        ret = QImage(img_width, img_width, QImage::Format_Mono); //mono位图
        QPainter painter(&ret);
        painter.fillRect(0, 0, img_width, img_width, Qt::white);//背景填充白色
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black); //黑色方块
        for (int y = 0; y < qr->width; y++) //行
        {
            for (int x = 0; x < qr->width; x++) //列
            {
                if (qr->data[y * qr->width + x] & 1) //1表示黑块
                {
                    QRect r(x * scale, y * scale, scale, scale);
                    painter.drawRect(r);
                }
            }
        }
        QRcode_free(qr);
    }
    return ret;
}
