#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QImage>
#include <QTime>
#include <QElapsedTimer>
#include <QDebug>
#include <thread>
#include <vector>
#include <functional>
#include "fpng.h"
#include "lodepng.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::test);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test()
{
    QString filepath = QFileDialog::getOpenFileName(this);
    if (filepath.isEmpty())
        return;
    qDebug() << filepath;
    std::thread td([=]{
        qDebug() << "started";
        QString dir = QApplication::applicationDirPath();

        auto calc_elapse = [=](std::function<void()> func, const QString &tag, const QString &path){
            QElapsedTimer timer;
            timer.start();
            func();
            auto elapse = timer.elapsed();
            qDebug() << tag << "-using" << elapse << "ms" << "-size" << QFile(path).size() / 1024.0 / 1024.0 << "M bytes";
        };

        // 先读取一张图片用于测试
        // 注意 QImage 四字节对齐但是有的 png 库没处理对齐，所以选择一张 width 是 4 的倍数的图
        QImage image(filepath);
        image.convertTo(QImage::Format_RGB888);
        QImage temp;
        QString path;

        // Qt bmp 保存测试
        path = dir + "/qt bmp.bmp";
        calc_elapse([&]{
            image.save(path, "bmp");
        }, "qt save bmp", path);

        // Qt bmp 读取测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load bmp", path);

        // Qt png 保存测试
        path = dir + "/qt png.png";
        calc_elapse([&]{
            image.save(path, "png");
        }, "qt save png", path);

        // Qt png 读取测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load png", path);

        // Qt png 100 质量保存测试
        path = dir + "/qt png 100.png";
        calc_elapse([&]{
            image.save(path, "png", 100);
        }, "qt save png 100 quality", path);

        // Qt png 100 质量读取测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load png 100 quality", path);

        // Qt jpg 保存测试
        path = dir + "/qt jpg.jpg";
        calc_elapse([&]{
            image.save(path, "jpg");
        }, "qt save jpg", path);

        // Qt jpg 读取测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load jpg", path);

        // Qt jpg 100 质量保存测试
        path = dir + "/qt jpg 100.jpg";
        calc_elapse([&]{
            image.save(path, "jpg");
        }, "qt save jpg 100 quality", path);

        // Qt jpg 100 质量读取测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load jpg 100 quality", path);

        // fpng
        // https://github.com/richgel999/fpng
        // fpng 保存测试
        path = dir + "/fpng.png";
        calc_elapse([&]{
            QByteArray localpath = path.toLocal8Bit();
            // 即使不是提前初始化，启用 sse4 也是很快的
            // fpng::fpng_init();
            bool ret = fpng::fpng_encode_image_to_file(localpath.data(), image.bits(), image.width(), image.height(), 3);
            if (!ret) {
                qDebug() << "fpng encode error";
            }
        }, "save fpng", path);

        // fpng 读取测试
        calc_elapse([&]{
            QByteArray localpath = path.toLocal8Bit();
            std::vector<uint8_t> buffer;
            uint32_t w, h, n;
            // fpng 读取测试，注意 fpng 解码不是完整支持，所以只对 fpng 编码的文件解码
            int ret = fpng::fpng_decode_file(localpath.data(), buffer, w, h, n, 3);
            if (ret != fpng::FPNG_DECODE_SUCCESS) {
                qDebug() << "fpng decode error";
            } else {
                // 读取后转为 QImage 算一个完整流程
                temp = QImage(buffer.data(), w, h, w * 3, QImage::Format_RGB888).copy();
            }
        }, "load fpng", path);

        // Qt 读取 fpng 测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load fpng", path);

        // lodepng
        // https://github.com/lvandeve/lodepng
        // lodepng 保存测试
        path = dir + "/lodepng.png";
        calc_elapse([&]{
            QByteArray localpath = path.toLocal8Bit();
            // returns 1 if success, 0 if failure
            int ret = lodepng::encode(localpath.data(), image.bits(), image.width(), image.height(), LCT_RGB, 8);
            if (ret != 0) {
                qDebug() << "lodepng encode error";
            }
        }, "save lodepng", path);

        // lodepng 读取测试
        calc_elapse([&]{
            QByteArray localpath = path.toLocal8Bit();
            std::vector<uint8_t> buffer;
            uint32_t w, h;
            int ret = lodepng::decode(buffer, w, h, localpath.toStdString(), LCT_RGB, 8);
            if (ret != 0) {
                qDebug() << "lodepng decode error";
            } else {
                // 读取后转为 QImage 算一个完整流程
                temp = QImage(buffer.data(), w, h, w * 3, QImage::Format_RGB888).copy();
            }
        }, "load lodepng", path);

        // Qt 读取 lodepng 测试
        calc_elapse([&]{
            temp = QImage(dir + "/lodepng.png");
        }, "qt load lodepng", path);

        // stb_image
        // https://github.com/nothings/stb
        // stb 保存测试
        path = dir + "/stb.png";
        calc_elapse([&]{
            QByteArray localpath = path.toLocal8Bit();
            // Each function returns 0 on failure and non-0 on success.
            int ret = stbi_write_png(localpath.data(), image.width(), image.height(), 3, image.bits(), image.bytesPerLine());
            if (ret == 0) {
                qDebug() << "stb encode error";
            }
        }, "save stb", path);

        // stb 读取测试
        calc_elapse([&]{
            QByteArray localpath = path.toLocal8Bit();
            int w, h, n;
            unsigned char *data = stbi_load(localpath.data(), &w, &h, &n, 3);
            if (!data) {
                qDebug() << "stb decode error";
            } else {
                // 读取后转为 QImage 算一个完整流程
                temp = QImage(data, w, h, w * 3, QImage::Format_RGB888).copy();
            }
            stbi_image_free(data);
        }, "load stb", path);

        // Qt 读取 stb 测试
        calc_elapse([&]{
            temp = QImage(path);
        }, "qt load stb", path);

        qDebug() << "finished";
    });
    td.detach();
}
